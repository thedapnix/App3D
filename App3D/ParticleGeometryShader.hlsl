cbuffer CAMERA_CONSTANT_BUFFER : register(b0)
{
    matrix view;
    matrix proj;
    float3 cameraPosition;
};

struct GeometryShaderOutput
{
	float4 pos : SV_POSITION;
};

[maxvertexcount(6)]
void main(
	point float3 input[1] : POSITION, 
	inout TriangleStream<GeometryShaderOutput> output
)
{
	/*Notes from the cookbook:
	Calculate front vector using input and camera position
	Use front vector along with "defaulted" up vector (0.0f, 1.0f, 0.0f) to calculate right vector
	Use front vector along with right vector to calculate our *actual* up vector
	Scale right and up based on preference
	*/
	//Perpendicular vector obtained from cross product (don't forget to normalize vectors)
    float3 frontVec = normalize(cameraPosition - input[0]);
    float3 upVec = float3(0.0f, 1.0f, 0.0f);
    float3 rightVec = normalize(cross(frontVec, upVec));
    upVec = normalize(cross(rightVec, frontVec));
	//As for "scale right and up based on preference" i'm not sure. make particles larger? maybe affect value through imgui?
    rightVec *= 0.1f;
    upVec *= 0.1;
	
	//Multiply the view and projection matrices from our camera constant buffer to get vp
    matrix vp = mul(view, proj);
	
    GeometryShaderOutput element;
	//Append elements in clockwise order just as we do on the cpu-side, so top left -> bottom right -> bottom left
    element.pos = mul(float4(input[0] - rightVec + upVec, 1.0f), vp);
    output.Append(element);
    element.pos = mul(float4(input[0] + rightVec - upVec, 1.0f), vp);
    output.Append(element);
    element.pos = mul(float4(input[0] - rightVec - upVec, 1.0f), vp);
    output.Append(element);
    output.RestartStrip(); //First triangle done
	
	//Append elements for the second triangle, once again in clockwise order. So now we'll do top left -> top right -> bottom right
    element.pos = mul(float4(input[0] - rightVec + upVec, 1.0f), vp);
    output.Append(element);
    element.pos = mul(float4(input[0] + rightVec + upVec, 1.0f), vp);
    output.Append(element);
    element.pos = mul(float4(input[0] + rightVec - upVec, 1.0f), vp);
    output.Append(element);
	
	//for (uint i = 0; i < 3; i++)
	//{
 //     GeometryShaderOutput element;
	//	element.pos = input[i];
	//	output.Append(element);
	//}
}