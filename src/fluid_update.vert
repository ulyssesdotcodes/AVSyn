uniform vec2 invresolution;
uniform sampler2D velocity;
uniform sampler2D target;
uniform sampler2D pressure;
uniform sampler2D divergence;
uniform float dt;
uniform float rdx;
uniform float alpha;

in vec2 texelCoord;
in vec2 p;

out vec4 fragColor;

// Might not work because we'll be working on last frame's values

vec2 simToTexelSpace(vec2 simSpace){
    return vec2(simSpace.x / aspectRatio + 1.0 , simSpace.y + 1.0)*.5;
}

vec4 advect() {
	vec2 tracedPos = p - dt * rdx * texture2D(velocity, texelCoord).xy;

	tracedPos = simToTexelSpace(tracedPos)/invresolution;

	vec4 st;
	st.xy = floor(tracedPos-0.5)+0.5;
	st.zw = st.xy+1.0;

	vec2 t = tracedPos - st.xy;

	vec4 tex11 = texture2D(target, st.xy);
	vec4 tex12 = texture2D(target, st.zy);
	vec4 tex21 = texture2D(target, st.xw);
	vec4 tex22 = texture2D(target, st.zw);

	return mix(mix(tex11, tex21, t.x), mix(tex12, tex22, t.x), t.y);
}

vec4 diverge(vec4 u) {
	float L = samplePressure(pressure, texelCoord - vec2(invresolution.x, 0));
	float R = samplePressure(pressure, texelCoord + vec2(invresolution.x, 0));
	float B = samplePressure(pressure, texelCoord - vec2(0, invresolution.y));
	float T = samplePressure(pressure, texelCoord + vec2(0, invresolution.y));

	float bC = u.x;

	return vec4( (L + R + B + T + alpha * bC) * .25, 0, 0, 1 );//rBeta = .25
}

void main() {
	vec4 u = advect();
	u = diverge(u);

}