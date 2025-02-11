/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#include "eh.hpp"

#include <QImage>

#include <boost/compute/algorithm/copy.hpp>
#include <boost/compute/system.hpp>
#include <boost/compute/types/fundamental.hpp>
#include <boost/compute/utility/source.hpp>

#include <concepts>
#include <print>
#include <random>
#include <ranges>


using namespace core;

namespace
{
    template <std::unsigned_integral T>
    T toIndex(T row, T col, T width)
    {
        return row * width + col;
    }


    const char cl_source[] =
    #include "kernels.cl"
}

std::vector<f32> core::normalizedValues(const QImage& image)
{
    const u32 width  = image.width();
    const u32 height = image.height();

    auto values = std::vector(width * height, f32(0));

    for (u32 row = 0; row < height; ++row) {
        for (u32 col = 0; col < width; ++col) {
            values[toIndex(row, col, width)] = image.pixelColor(col, row).valueF();
        }
    }

    const auto minv = *std::ranges::min_element(values);
    const auto maxv = *std::ranges::max_element(values);

    if (std::fabs(maxv - minv) > 0) {
        const auto rangeInv = 1.f / (maxv - minv);
        auto normalize = [minv, rangeInv](auto x) {
            return x * rangeInv - minv * rangeInv;
        };

        for (auto& x : values) { x = normalize(x); }
    }

    assert(*std::ranges::min_element(values) >= 0.0);
    assert(*std::ranges::max_element(values) <= 1.0);

    return values;
}

ElectrostaticHalftoning::ElectrostaticHalftoning(QObject* parent)
    : QObject(parent)
{
    auto device = compute::system::default_device();

    if (device.type() == CL_DEVICE_TYPE_GPU) {
        std::println("\ngpu: {}", device.name());
        std::println("driver: {}", device.driver_version());
        std::println("platform: {}", device.platform().name());
        std::println("{}", device.platform().version());
        std::println("compute units: {}", device.compute_units());

        _context = compute::context(device);
        _queue   = compute::command_queue(_context, device);
        _program = compute::program::create_with_source(cl_source, _context);
        _program.build();

        _values_dev   = compute::vector<f32>(1, _context);
        _forceField   = compute::vector<compute::float2_>(1, _context);
        _particles_k0 = compute::vector<compute::float2_>(1, _context);
        _particles_k1 = compute::vector<compute::float2_>(1, _context);
        _shake        = compute::vector<compute::float2_>(1, _context);
    }
}

void ElectrostaticHalftoning::setValues(const std::vector<f32>& values, u32 width, u32 height)
{
    assert(values.size() == width * height);
    assert(std::ranges::all_of(values, [](auto x) { return x >= f32(0) && x <= f32(1); }));

    _width  = width;
    _height = height;
    _values = values;

    _values_dev.resize(_values.size());
    compute::copy(_values.begin(), _values.end(), _values_dev.begin(), _queue);

    computeForceField();
    reset();
}

void ElectrostaticHalftoning::setParticleCount(i32 count)
{
    _particleCount = std::max(1, count);
    reset();
}

void ElectrostaticHalftoning::setParticleRadius(f32 radius)
{
    if (radius != _radius && _radius > 0) {
        _radius = radius;
        reset();
    }
}

void ElectrostaticHalftoning::setMaxIteration(i32 i)
{
    _maxIterations = std::max(1, i);
    reset();
}

void ElectrostaticHalftoning::nextIteration()
{
    if (_currentIteration >= _maxIterations) {
        return;
    }
    _currentIteration++;

    if (_currentIteration%10 == 0) {
        shake();
    }

    compute::float2_ boundry{_width - 1.f , _height - 1.f };

    _iterateKernel = _program.create_kernel("iterate");
    _iterateKernel.set_arg(0, _particles_k0.get_buffer());
    _iterateKernel.set_arg(1, _particles_k1.get_buffer());
    _iterateKernel.set_arg(2, _forceField.get_buffer());
    _iterateKernel.set_arg(3, _width);
    _iterateKernel.set_arg(4, boundry);
    _iterateKernel.set_arg(5, _radius);

    _queue.enqueue_1d_range_kernel(_iterateKernel, 0, _particles_k0.size(), 0).wait();
    _queue.finish();

    updateResult();
    _particles_k0.swap(_particles_k1);

    emit iterationFinished(_results, _currentIteration, _maxIterations);
}

void ElectrostaticHalftoning::updateResult()
{
    struct Pusher
    {
        using value_type = std::vector<compute::float2_>::value_type;

        Pusher(QVector<QPointF>& destination) : _d(&destination) { }

        void push_back(const value_type& value) const
        {
            _d->push_back(QPointF(value.x, value.y));
        }
        QVector<QPointF>* _d;
    };

    _results.clear();
    _results.resize(_particles_k0.size());

    auto pusher = Pusher(_results);

    compute::copy(_particles_k0.begin(), _particles_k0.end(), std::back_inserter(pusher), _queue);
}

void ElectrostaticHalftoning::computeForceField()
{
    _forceField.resize((_width + 2) * (_height + 2));
    compute::fill(_forceField.begin(), _forceField.end(), compute::float2_(0, 0), _queue);

    _forceFieldKernel = _program.create_kernel("computeForceField");
    _forceFieldKernel.set_arg(0, _values_dev.get_buffer());
    _forceFieldKernel.set_arg(1, _forceField.get_buffer());
    _forceFieldKernel.set_arg(2, _width);
    _forceFieldKernel.set_arg(3, _height);
    _queue.enqueue_1d_range_kernel(_forceFieldKernel, 0, _width*_height, 0).wait();
    _queue.finish();
}

void ElectrostaticHalftoning::initializeParticles(i32 count)
{
    assert(count > 0);

    std::uniform_real_distribution<f32> uniform01(0, 1);
    std::uniform_int_distribution<u32> uidW(0, _width-1);
    std::uniform_int_distribution<u32> uidH(0, _height-1);
    std::minstd_rand0 rng1(time(0));
    std::minstd_rand0 rng2(time(0) + 1);

    std::vector<compute::float2_> tmp; tmp.reserve(count);

    i32 i = count;
    while (i > 0) {
        const auto col = uidW(rng1);
        const auto row = uidH(rng1);
        const auto val = _values[toIndex(row, col, _width)];

        if (uniform01(rng2) >= val) {
            i--;
            f32 x = col + uniform01(rng2);
            f32 y = row + uniform01(rng2);

            assert(x >= 0 && x < _width);
            assert(y >= 0 && y < _height);

            tmp.push_back({x, y});
        }
    }

    _results.resize(count);
    _particles_k0.resize(count);
    _particles_k1.resize(count);

    compute::copy(tmp.begin(), tmp.end(), _particles_k0.begin(), _queue);
}

void ElectrostaticHalftoning::shake()
{
    const auto c1 = std::max(0.0, (std::log2f(_maxIterations) - 6.0) / 10.0);
    const auto mag = c1 * std::exp(-(_currentIteration+1) / 1000.0);
    const auto size = _particles_k0.size();

    std::uniform_real_distribution<f32> urd(0, 1);
    std::minstd_rand rng(time(0));

    std::vector<compute::float2_> tmp; tmp.reserve(_particles_k0.size());
    for (auto i : std::ranges::iota_view{0u, size}) {
        auto dis = compute::float2_(urd(rng) * mag, urd(rng) * mag);
        tmp.push_back(dis);
    }
    _shake.resize(_particles_k0.size());
    compute::copy(tmp.begin(), tmp.end(), _shake.begin(), _queue);

    _shakeKernel = _program.create_kernel("shake");
    _shakeKernel.set_arg(0, _particles_k0.get_buffer());
    _shakeKernel.set_arg(1, _shake.get_buffer());
    _queue.enqueue_1d_range_kernel(_shakeKernel, 0, _particles_k0.size(), 0).wait();
    _queue.finish();
}

void ElectrostaticHalftoning::reset()
{
    _currentIteration = 0;
    initializeParticles(_particleCount);
}