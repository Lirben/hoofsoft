class HoofSerial
{
  public:
    void setSerial(Stream &serial);
    bool dataAvailable();
    String readln();
    void println(String payload);

  private:
    Stream* _serial;
    String _serialInput;
};

