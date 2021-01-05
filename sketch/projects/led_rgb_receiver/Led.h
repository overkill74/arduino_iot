
#ifndef LED_H
#define LED_H

class Ticker;

///
/// \brief Classe LED
///
class Led
{
  public:
    Led(const char* name, int pin, int max_val=255, bool invert=false, int ticker_ms=25, int raise_ms=2500);
    ///
    /// \brief Imposta la luminosita'
    ///
    void setLuminosity(int luma);
    ///
    /// \brief Print status
    ///
    void printStatus();

  private:
    ///
    /// \brief Applica m_luma all'uscita
    ///
    void drive_output();
    ///
    /// \brief Loop per impostare la luninosita'
    ///
    void do_work();

  private:
    int         m_pin;            //! Il Pin utilizzato
    const char* m_name;           //! Name of LED
    float       m_luma;           //! Luminosita'
    float       m_luma_target;    //! Obiettivo Luminosita'
    float       m_luma_delta;     //! Delta
    int         m_luma_max;       //! Massimo valore luminosita'
    bool        m_invert;         //! Inverte l'uscita
    Ticker*     m_ticker;         //! Ticker
    float       m_ticker_steps;   //! Ticker steps
    bool        m_tr_done;        //! Transaction was done
};

#endif
