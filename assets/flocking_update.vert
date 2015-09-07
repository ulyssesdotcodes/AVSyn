#version 330 core

layout (location = 0) in vec3 iPosition;	// POSITION_INDEX
layout (location = 1) in vec3 iVelocity;			// VELOCITY_INDEX
layout (location = 2) in vec3 iColor;

uniform samplerBuffer tex_position;
uniform samplerBuffer tex_velocity;

uniform vec2 ciPosition;

uniform float delta;
uniform float separationDistance;
uniform float alignmentDistance;
uniform float cohesionDistance;
uniform float loudness;
uniform float beat;
uniform float accumulatedLoudness;
uniform float roamingDistance;
uniform float speed;
uniform float eqs[3];
uniform float hue;
uniform float saturation;
uniform bool separateOnly;

out vec3 tf_position;
out vec3 tf_velocity;
out vec3 tf_color;

const float PI = 3.141592654;
const float PI_2 = PI * 2.0;

const float WIDTH = 64.0;

float separationThresh = 0.45;
float alignmentThresh = 0.65;

const float UPPER_BOUNDS = 400.0;
const float LOWER_BOUNDS = -UPPER_BOUNDS;

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 rgb2hsv(vec3 c) {
  vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
  vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
  vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

  float d = q.x - min(q.w, q.y);
  float e = 1.0e-10;
  return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 calculatePosition(vec3 position, vec3 velocity) {
  float delt;
  if(delta > 1.0) {
    delt = 1.0;
  }
  else {
    delt = delta;
  }
  return position.xyz + velocity * delt * 12.0;
}

vec4 calculateVelocity(vec3 position, vec3 velocity, float prevHue) {
  float delt = min(1.0, delta);
  float cd = cohesionDistance * (0.6 + 0.2 * sin(accumulatedLoudness * 0.05) + eqs[2]);
  float sd = separationDistance * (0.4 + cos(accumulatedLoudness * 0.05) + beat * loudness + eqs[1]);
  float ad = alignmentDistance * (0.6 + eqs[0]);

  float zoneRadius = sd + ad + cd;
  separationThresh = sd / zoneRadius;
  alignmentThresh = (sd + ad) / zoneRadius;
  float zoneRadiusSquared = zoneRadius * zoneRadius;

  vec3 pointPosition, pointVelocity;
  float hueVelocity;

  float dist;
  vec3 dir;
  float distSquared;

  float separationSquared = separationDistance * separationDistance;
  float cohesionSquared = cohesionDistance * cohesionDistance;

  float percent;

  float selfHueVelocity = 0.0;

  //TODO: Predator

  vec3 central = vec3(0.0, 0.0, 0.0);
  dir = position.xyz - central;
  dist = length(dir);
  distSquared = dist * dist;

  vec3 norm = vec3(
    step(roamingDistance, abs(position.x)) * sign(position.x),
     step(roamingDistance, abs(position.y)) * sign(position.y),
     step(roamingDistance, abs(position.z)) * sign(position.z));

  if(length(norm) > 0.0 && dot(velocity, norm) > 0.0) {
    velocity = reflect(velocity, norm);
  }

  velocity -= normalize(dir) * delt * 2.0;

  float separationCount, alignmentCount, cohesionCount;

  separationCount = 0.0;
  alignmentCount = 0.0;
  cohesionCount = 0.0;

  for(float y=0.0; y < WIDTH; y++) {
    for(float x = float(int(accumulatedLoudness) % 3); x < WIDTH; x++){
	  if(int(x) % 3 == 0) continue;
      pointPosition = texelFetch(tex_position, int(y * WIDTH + x)).xyz;
      pointVelocity = texelFetch(tex_velocity, int(y * WIDTH + x)).xyz;
      dir = pointPosition.xyz - position.xyz;
      dist = length(dir);
	  if(dist < 0.0001f) {
		  continue;
	  }
      distSquared = dist * dist;

	  float f = 0.25;

      if(dist > 0.0 && distSquared < zoneRadiusSquared) {
        percent = distSquared / zoneRadiusSquared;

        if(percent < separationThresh) {
          // Separate
          f *= (separationThresh / percent - 1.0) * delt * 0.7;
          velocity -= normalize(dir) * f;
          selfHueVelocity += 0.0;
          separationCount += 1.0;
        }
        else if (percent < alignmentThresh && !separateOnly){
          // Align
          float threshDelta = alignmentThresh - separationThresh;
          float adjustedPercent = (percent - separationThresh) / threshDelta;

          f *= (0.5 - cos(adjustedPercent * PI_2) * 0.5 + 0.5) * delt * 0.7;
          velocity += normalize(pointVelocity.xyz) * f;
          selfHueVelocity += 0.33;
          alignmentCount += 1.0;
        }
        else if(!separateOnly) {
          // Cohese
          float threshDelta = 1.0 - alignmentThresh;
          float adjustedPercent = (percent - alignmentThresh) / threshDelta;
          f  *= (0.5 - cos(adjustedPercent * PI_2) * -0.5 + 0.5) * delt * 0.7;
          velocity += normalize(dir) * f;
          selfHueVelocity += 0.66;
          cohesionCount += 1.0;
        }
      }
    }
  }

  float zoneCount = separationCount + alignmentCount + cohesionCount;

  // Calculate the hue based on how it's being affected by other points
  if (alignmentCount > separationCount && alignmentCount > cohesionCount){
    selfHueVelocity = 0.33;
  }
  else if (alignmentCount < separationCount && separationCount > cohesionCount){
    selfHueVelocity = 0.0;
  }
  else if (cohesionCount > separationCount && alignmentCount < cohesionCount){
    selfHueVelocity = 0.66;
  }

  if(zoneCount == 0.0) {
    selfHueVelocity = prevHue;
  }
  else {
    selfHueVelocity =  mix(prevHue, selfHueVelocity, delt * 10.0);
  }

  if(length(velocity) > speed) {
    velocity = normalize(velocity) * speed;
  }

  return vec4(velocity * (0.99 + beat*beat*0.5), selfHueVelocity);
}

void main() {
	vec3 color = rgb2hsv(iColor);
	color.x = fract(color.x - hue);
	vec4 velocity = calculateVelocity(iPosition, iVelocity.xyz, color.x);
	vec3 position = calculatePosition(iPosition, iVelocity.xyz);
	tf_position = position;
	tf_velocity = velocity.xyz;
	tf_color = hsv2rgb(vec3(fract(velocity.w + hue), saturation, 1.0));
}