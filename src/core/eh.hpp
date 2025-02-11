/// Copyright (C) 2025 Arlen Avakian
/// SPDX-License-Identifier: GPL-3.0-or-later


#pragma once

/// based on Electrostatic Halftoning
/// https://www.mia.uni-saarland.de/Research/Electrostatic_Halftoning/model.shtml
///


#include <QObject>

#include <boost/compute/container/vector.hpp>

#include <vector>


namespace compute = boost::compute;


using f32 = float;
using f64 = double;
using u32 = std::uint32_t;
using i32 = std::int32_t;


namespace core
{
    std::vector<f32> normalizedValues(const QImage& image);

    class ElectrostaticHalftoning final : public QObject
    {
        Q_OBJECT

    signals:
        void iterationFinished(const QVector<QPointF>& points, int iter, int iterMax);

    public:
        ElectrostaticHalftoning(QObject* parent = nullptr);

        i32 currentIteration() const { return _currentIteration; }

        i32 maxIterations() const { return _maxIterations; }

        void setValues(const std::vector<f32>& values, u32 width, u32 height);

        void setParticleCount(i32 count);

        void setParticleRadius(f32 radius);

        void setMaxIteration(i32 i);

        void nextIteration();

    private:
        void updateResult();

        void computeForceField();

        void initializeParticles(i32 count);

        void shake();

        void reset();


        i32 _particleCount{1024*4};
        i32 _currentIteration{0};
        i32 _maxIterations{16};
        u32 _width{1};
        u32 _height{1};
        f32 _radius{1};

        compute::vector<compute::float2_> _forceField;
        compute::vector<compute::float2_> _particles_k0;
        compute::vector<compute::float2_> _particles_k1;
        compute::vector<compute::float2_> _shake;

        std::vector<f32> _values;
        compute::vector<f32> _values_dev;
        QVector<QPointF> _results;

        compute::context _context;
        compute::command_queue _queue;
        compute::program _program;

        compute::kernel _forceFieldKernel;
        compute::kernel _iterateKernel;
        compute::kernel _shakeKernel;
    };
}