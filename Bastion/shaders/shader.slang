struct VertexOutput {
    float3 color : COLOR;
    float4 sv_position : SV_Position;
};

struct PushConstants {
    float angle;
};

[[vk::push_constant]] PushConstants pc;

[shader("vertex")]
VertexOutput vertMain(uint vid : SV_VertexID) {
    float2 positions[3] = {
        float2(0.0, -0.5),
        float2(0.5, 0.5),
        float2(-0.5, 0.5)
    };

    float3 colors[3] = {
        float3(1.0, 1.0, 1.0),
        float3(1.0, 1.0, 1.0),
        float3(1.0, 1.0, 1.0)
    };

    float2 p = positions[vid];
    float s, c;
    sincos(pc.angle, s, c);

    p = float2(c * p.x - s * p.y, s * p.x + c * p.y);

    VertexOutput output;
    output.sv_position = float4(p, 0.0, 1.0);
    output.color = colors[vid];
    return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target {
    return float4(inVert.color, 1.0);
}
