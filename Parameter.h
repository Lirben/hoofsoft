#ifndef PARAMETER_H
#define PARAMETER_H

class Parameter
{
  public:
    Parameter(String key, String value);
    bool readBlValue();
    int readIntValue();
    String readStrValue();
    String readKey();
    
  private:
    String _key;
    int _intValue;
    bool _blValue;
    String _strValue;
};

#endif
