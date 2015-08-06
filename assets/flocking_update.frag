uniform float delta;
uniform float separationDistance;
uniform float alignmentDistance;
uniform float cohesionDistance;
uniform float loudness;
uniform float beat;
uniform float accumulatedLoudness;
uniform float roamingDistance;
uniform float speed;
uniform vec3 eqs;
uniform vec2 resolution;

uniform sampler2D iPositionTexture;
uniform sampler2D iVelocityTexture;

const float PI = 3.141592654;
const float PI_2 = PI * 2.0;

const float WIDTH = 64.0;

float separationThresh = 0.45;
float alignmentThresh = 0.65;

const float UPPER_BOUNDS = 400.0;
const float LOWER_BOUNDS = -UPPER_BOUNDS;

out vec4 oPosition;
out vec4 oVelocity;

vec3 calculatePosition(vec3 position, vec3 velocity) {
  float delt;
  if(delta > 1.0) {
    delt = 1.0;
  }
  else {
    delt = delta;
  }
  return position + velocity * delt * 12.0;
}

vec4 calculateVelocity(vec3 position, vec4 velocityHue) {
  float delt = min(1.0, delta);
  float cd = cohesionDistance * (0.6 + 0.2 * sin(accumulatedLoudness * 0.25) + eqs.z);
  float sd = separationDistance * (0.4 + cos(accumulatedLoudness * 0.25) + eqs.x);
  float ad = alignmentDistance * (0.6 + eqs.y);

  float zoneRadius = sd + ad + cd;
  separationThresh = sd / zoneRadius;
  alignmentThresh = (sd + ad) / zoneRadius;
  float zoneRadiusSquared = zoneRadius * zoneRadius;

  vec2 uv = gl_FragCoord.xy / resolution.xy;
  vec4 pointPosition, pointVelocity;
  float hueVelocity;

  float dist;
  vec3 dir;
  float distSquared;

  float separationSquared = separationDistance * separationDistance;
  float cohesionSquared = cohesionDistance * cohesionDistance;

  float percent;

  float selfHueVelocity = 0.0;
  vec3 velocity = velocityHue.xyz;

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
    for(float x = 0.0; x < WIDTH; x++){
      if(x == gl_FragCoord.x && y == gl_FragCoord.y) {
        continue;
      }

      pointPosition = texture2D(iPositionTexture, vec2(x / resolution.x, y / resolution.y));
      pointVelocity = texture2D(iVelocityTexture, vec2(x / resolution.x, y / resolution.y));
      dir = pointPosition.xyz - position.xyz;
      dist = length(dir);
      distSquared = dist * dist;

      float f = loudness * 0.25;

      if(dist > 0.0 && distSquared < zoneRadiusSquared) {
        percent = distSquared / zoneRadiusSquared;

        if(percent < separationThresh) {
          // Separate
          f *= (separationThresh / percent - 1.0) * delt * 0.7;
          velocity -= normalize(dir) * f;
          selfHueVelocity += 0.0;
          separationCount += 1.0;
        }
        else if (percent < alignmentThresh){
          // Align
          float threshDelta = alignmentThresh - separationThresh;
          float adjustedPercent = (percent - separationThresh) / threshDelta;

          f *= (0.5 - cos(adjustedPercent * PI_2) * 0.5 + 0.5) * delt * 0.7;
          velocity += normalize(pointVelocity.xyz) * f;
          selfHueVelocity += 0.33;
          alignmentCount += 1.0;
        }
        else {
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

  if (alignmentCount > separationCount && alignmentCount > cohesionCount){
    selfHueVelocity = 0.33;
  }
  else if (alignmentCount < separationCount && separationCount > cohesionCount){
    selfHueVelocity = 1.0;
  }
  else if (cohesionCount > separationCount && alignmentCount < cohesionCount){
    selfHueVelocity = 0.66;
  }


  if(zoneCount == 0.0) {
    selfHueVelocity = velocityHue.w;
  }
  else {
    selfHueVelocity =  mix(velocityHue.w, selfHueVelocity, delt * 10.0);
  }

  if(length(velocity) > speed) {
    velocity = normalize(velocity) * speed;
  }

  return vec4(velocity, selfHueVelocity);
}

void main() {
  vec2 uv = gl_FragCoord.xy / resolution.xy;
  vec4 position = vec4(texture2D(iPositionTexture, uv).xyz, 0);
  vec4 velocity = texture2D(iVelocityTexture, uv);

  oPosition = vec4(calculatePosition(position.xyz, velocity.xyz), velocity.w);
  oVelocity = calculateVelocity(oPosition.xyz, velocity);
}