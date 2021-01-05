#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "Led.h"

//---------------------------------------------------
Led::Led(const char* name, int pin, int max_val, bool invert, int ticker_ms, int raise_ms)
  : m_pin(pin)
  , m_name(name)
  , m_luma(0)
  , m_luma_target(0)
  , m_luma_delta(1)
  , m_luma_max(max_val)
  , m_invert(invert)
  , m_ticker(new Ticker)
  , m_ticker_steps((float)(raise_ms / ticker_ms))
  , m_tr_done(true)
{
  pinMode(m_pin, OUTPUT);
  drive_output();
  m_ticker->attach_ms(ticker_ms, std::bind(&Led::do_work, this));
}

//---------------------------------------------------
void Led::drive_output()
{
  int ov = m_invert ? (m_luma_max - m_luma) : (m_luma);
  if (ov < 0) {
    ov = 0;
  }
  if (ov > m_luma_max) {
    ov = m_luma_max;
  }
  analogWrite(m_pin, ov);
}

//---------------------------------------------------
void Led::setLuminosity(int luma)
{
  if (luma < 0) { luma = 0; }
  if (luma > m_luma_max) { luma = m_luma_max; }
  m_luma_target = (float)luma;
  m_luma_delta = fabs(m_luma_target - m_luma) / m_ticker_steps;
  m_tr_done = false;
}

//---------------------------------------------------
void Led::printStatus()
{
  Serial.println("\n-------------------------");
  Serial.print("m_name         : "); Serial.println(m_name        );
  Serial.print("m_luma         : "); Serial.println(m_luma        );
  Serial.print("m_luma_target  : "); Serial.println(m_luma_target );
  Serial.print("m_luma_delta   : "); Serial.println(m_luma_delta  );
  Serial.print("m_luma_max     : "); Serial.println(m_luma_max    );
  Serial.print("m_invert       : "); Serial.println(m_invert      );
  Serial.print("m_ticker_steps : "); Serial.println(m_ticker_steps);
  Serial.print("m_tr_done      : "); Serial.println(m_tr_done     );
}

//---------------------------------------------------
void Led::do_work()
{
  if (m_tr_done) {
    return;
  }

  if (m_luma_target >= m_luma) {
    m_luma += m_luma_delta;
    if (m_luma > m_luma_target) {
      m_luma = m_luma_target;
      m_tr_done = true;
    }
  }
  else if (m_luma_target < m_luma) {
    m_luma -= m_luma_delta;
    if (m_luma < m_luma_target) {
      m_luma = m_luma_target;
      m_tr_done = true;
    }
  }
  
  drive_output();
}
