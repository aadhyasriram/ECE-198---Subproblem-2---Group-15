// ECE 198 Solution 1: controlled wrist-worn biceps curl demonstration.
#include <Wire.h>
#include "CurlCounter.h"
using curl::Vec;
const uint8_t SENSOR_ADDR = 0x68;
const uint32_t PERIOD_MS=20;
curl::Counter counter;
Vec baseline{}, filtered{}, first{};
unsigned stable=0;
bool calibrated=false, filterReady=false;
uint32_t last=0, lastLog=0;

bool regWrite(uint8_t reg,uint8_t value) {
  Wire.beginTransmission(SENSOR_ADDR); Wire.write(reg); Wire.write(value);
  return Wire.endTransmission()==0;
}
bool burst(uint8_t reg,uint8_t* buf,uint8_t n) {
  Wire.beginTransmission(SENSOR_ADDR); Wire.write(reg);
  if (Wire.endTransmission(false)!=0 || Wire.requestFrom(SENSOR_ADDR,n)!=n) return false;
  for (unsigned i=0;i<n;++i) buf[i]=Wire.read();
  return true;
}
int16_t wordAt(const uint8_t* p) { return int16_t((uint16_t(p[0])<<8)|p[1]); }
void invalidate() {
  counter.interrupt(); filterReady=false; stable=0;
}
void setup() {
  Serial.begin(115200);
  uint32_t begin=millis();
  while (!Serial && millis()-begin<2500) {}
  Wire.begin();
  uint8_t id=0;
  bool ok=burst(0x75,&id,1) && id==0x68 && regWrite(0x6B,1);
  delay(100);
  // 50 Hz, low-pass ~20 Hz, +/-4 g, +/-500 degrees/s.
  if (!ok || !regWrite(0x1A,4) || !regWrite(0x19,19) ||
      !regWrite(0x1B,8) || !regWrite(0x1C,8)) {
    for (;;) { Serial.println("# MPU6050 init failed: check address/wiring, then reset."); delay(1500); }
  }
  Serial.println("# Start with arm down, wrist neutral. Hold still for 2 seconds.");
}
void loop() {
  uint32_t now=millis(), dt=now-last;
  if (dt<PERIOD_MS) return;
  last=now;
  if (dt>100) invalidate();
  uint8_t b[14];
  if (!burst(0x3B,b,14)) {
    invalidate();
    if (now-lastLog>=1000) { lastLog=now; Serial.println("# Missing IMU data; return to start pose."); }
    return;
  }
  Vec a{wordAt(b)/8192.f,wordAt(b+2)/8192.f,wordAt(b+4)/8192.f};
  float mag=a.norm();
  if (!calibrated) {
    Vec gyro{wordAt(b+8)/65.5f,wordAt(b+10)/65.5f,wordAt(b+12)/65.5f};
    if (gyro.norm()>5 || fabsf(mag-1)>0.10f || (stable && curl::separation(a,first)>4)) {
      stable=0; return;
    }
    if (!stable) first=baseline=a;
    ++stable; baseline=baseline.blend(a,1.f/stable);
    if (stable>=100) {
      calibrated=true; filtered=baseline; filterReady=true;
      Serial.println("# Ready. Complete a curl, then return to the starting posture.");
      Serial.println("event,time_ms,reps,tilt_deg,rep_peak_deg,rep_duration_s");
    }
    return;
  }
  // Tilt from gravity is useful for slow curls. Reject strong linear acceleration.
  if (mag<0.75f || mag>1.25f) { invalidate(); return; }
  filtered=filterReady ? filtered.blend(a,float(dt)/(100.f+dt)) : a;
  filterReady=true;
  float angle=curl::separation(filtered,baseline);
  bool complete=counter.update(angle,now);
  if (complete || now-lastLog>=100) {
    lastLog=now;
    Serial.print(complete?"REP,":"SAMPLE,"); Serial.print(now); Serial.print(',');
    Serial.print(counter.reps); Serial.print(','); Serial.print(angle,1); Serial.print(',');
    if (complete) Serial.print(counter.peakDeg,1);
    Serial.print(',');
    if (complete) Serial.print(counter.durationMs/1000.f,2);
    Serial.println();
  }
}
