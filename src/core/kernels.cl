BOOST_COMPUTE_STRINGIZE_SOURCE(

/// given a position, compute the pull force using bilinear interpolation.
__kernel void computePullForce(__global const float2* forceField, const float2 pos, uint width, __local float2* output)
{
    float2 xy1  = floor(pos);
    float2 ones = {1, 1};
    float2 xy2  = floor(pos+ones);

    float4 weight = { (xy2.x - pos.x) * (xy2.y - pos.y)
                    , (xy2.x - pos.x) * (pos.y - xy1.y)
                    , (pos.x - xy1.x) * (xy2.y - pos.y)
                    , (pos.x - xy1.x) * (pos.y - xy1.y) };

    uint row = xy1.y;
    uint col = xy1.x;
    uint i1  = row     * width + col;
    uint i2  = (row+1) * width + col;

    *output = weight.x * forceField[i1  ]
            + weight.y * forceField[i1+1]
            + weight.z * forceField[i2  ]
            + weight.w * forceField[i2+1];
}

__kernel void shake(__global float2* points, __global float2* jitter)
{
    uint gid = get_global_id(0);

    points[gid] += jitter[gid];
}

__kernel void computeForceField(__global const float* values, __global float2* forceField, uint w, uint h)
{
    uint gid = get_global_id(0);

    if (gid >= w*h) {
        /// this should never happen!
        return;
    }

    float2 PosP;
    PosP.x = gid % w;
    PosP.y = gid / w;

    double2 totalForce = {0, 0};
    float2 PosG = {0, 0};

    for (uint rowG = 0; rowG < h; ++rowG) {
        for (uint colG = 0; colG < w; ++colG) {
            uint indexG = rowG * w + colG;
            if (indexG != gid) {
                float charge = 1.0f - values[indexG];
                float2 e_pg  = PosG - PosP;
                float force  = charge / dot(e_pg, e_pg);
                totalForce += convert_double2(force * normalize(e_pg));
            }
            PosG.x += 1;
        }
        PosG.x = 0;
        PosG.y += 1;
    }

    forceField[gid] = convert_float2(totalForce);
}

__kernel void iterate(__global float2* points, __global float2* result, __global const float2* forceField, uint w, float2 boundry, float radius)
{
    uint gid = get_global_id(0);
    uint N   = get_global_size(0);

    float2 Pn        = points[gid];
    float2 pushForce = {0, 0};

    for (uint j = 0; j < N; ++j) {
        if (j != gid) {
            float2 Pm = points[j];
            if (isequal(Pm.x, Pn.x) && isequal(Pm.y, Pn.y)) {
                continue;
            }
            float2 e_nm = Pm - Pn;
            float force = 1.0f / dot(e_nm, e_nm);
            pushForce += force * normalize(e_nm) ;
        }
    }

    __local float2 pullForce;
    computePullForce(forceField, Pn, w, &pullForce);

    float tau         = 0.1;
    float2 totalForce = (pullForce - pushForce * radius);
    float2 newPn      = Pn + totalForce * tau;

    float2 zero = {0, 0};

    if (newPn.x < 0 || newPn.y < 0 || newPn.x > boundry.x || newPn.y > boundry.y) {
        newPn = Pn;
    }

    result[gid] = newPn;
}


);