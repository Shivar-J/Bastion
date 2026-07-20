struct VertexOutput {
    float3 color : COLOR;
    float4 sv_position : SV_Position;
};

struct PushConstants {
    column_major float4x4 model;
    column_major float4x4 view;
    column_major float4x4 proj;
};

[[vk::push_constant]] PushConstants pc;

[shader("vertex")]
VertexOutput vertMain(uint vid : SV_VertexID) {
    float3 positions[36] = {
      float3(-0.5, -0.5,  0.5),
      float3( 0.5, -0.5,  0.5),
      float3( 0.5,  0.5,  0.5),
      float3(-0.5, -0.5,  0.5),
      float3( 0.5,  0.5,  0.5),
      float3(-0.5,  0.5,  0.5),

      float3( 0.5, -0.5, -0.5),
      float3(-0.5, -0.5, -0.5),
      float3(-0.5,  0.5, -0.5),
      float3( 0.5, -0.5, -0.5),
      float3(-0.5,  0.5, -0.5),
      float3( 0.5,  0.5, -0.5),

      float3(-0.5, -0.5, -0.5),
      float3(-0.5, -0.5,  0.5),
      float3(-0.5,  0.5,  0.5),
      float3(-0.5, -0.5, -0.5),
      float3(-0.5,  0.5,  0.5),
      float3(-0.5,  0.5, -0.5),

      float3(0.5, -0.5,  0.5),
      float3(0.5, -0.5, -0.5),
      float3(0.5,  0.5, -0.5),
      float3(0.5, -0.5,  0.5),
      float3(0.5,  0.5, -0.5),
      float3(0.5,  0.5,  0.5),

      float3(-0.5, 0.5,  0.5),
      float3( 0.5, 0.5,  0.5),
      float3( 0.5, 0.5, -0.5),
      float3(-0.5, 0.5,  0.5),
      float3( 0.5, 0.5, -0.5),
      float3(-0.5, 0.5, -0.5),

      float3(-0.5, -0.5, -0.5),
      float3( 0.5, -0.5, -0.5),
      float3( 0.5, -0.5,  0.5),
      float3(-0.5, -0.5, -0.5),
      float3( 0.5, -0.5,  0.5),
      float3(-0.5, -0.5,  0.5)
   };

  float3 faceColors[6] = {
      float3(1.0, 0.15, 0.15),
      float3(0.15, 1.0, 0.15),
      float3(0.15, 0.35, 1.0),
      float3(1.0, 0.85, 0.15),
      float3(1.0, 0.15, 1.0),
      float3(0.15, 1.0, 1.0)
   };

    VertexOutput output;
    float4 worldPosition = mul(pc.model, float4(positions[vid], 1.0));
    float4 viewPosition = mul(pc.view, worldPosition);
    output.sv_position = mul(pc.proj, viewPosition);
    output.color = faceColors[vid / 6];
    return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target {
    return float4(inVert.color, 1.0);
}
