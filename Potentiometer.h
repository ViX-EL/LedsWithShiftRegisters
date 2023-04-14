#pragma once
#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

class Potentiometer
{
public:
    Potentiometer(const int potPin, const int minVal, const int maxVal);

    static int read(const int potPin, const int minVal, const int maxVal);
    int read();

    bool isValWasChanged();
    bool isValWasChangedLeft();
    bool isValWasChangedRight();

    int getPrevState();
private:
    int m_potPin;
    int m_currState = 0, m_prevState = 0;
    int m_minVal = 0, m_maxVal = 9;
    
    bool m_valWasChanged = false;
    bool valWasChangedLeft = false, valWasChangedRight = false;
};

Potentiometer::Potentiometer(const int potPin, const int minVal, const int maxVal)
{
    m_potPin = potPin;
    m_minVal = minVal;
    m_maxVal = maxVal;
    pinMode(potPin, INPUT);
}

int Potentiometer::read(const int potPin, const int minVal, const int maxVal)
{
    int potVal = analogRead(potPin);
    potVal = map(potVal, 0, 1023, minVal, maxVal + 1);
    potVal = constrain(potVal, minVal, maxVal);
    return potVal;
}

int Potentiometer::read() 
{
    m_prevState = m_currState;
    m_currState = analogRead(m_potPin);
    if (m_prevState != m_currState)
    {
        m_valWasChanged = true;
        if(m_prevState < m_currState)
        {
            valWasChangedRight = true;
        }
        else
            valWasChangedLeft = true;
    }
    else
    {
        m_valWasChanged = false;
        valWasChangedLeft = false;
        valWasChangedRight = false;
    }
    
    m_currState = map(m_currState, 0, 1023, m_minVal, m_maxVal + 1);
    m_currState = constrain(m_currState, m_minVal, m_maxVal);
    return m_currState;
}

bool Potentiometer::isValWasChanged() 
{
    return m_valWasChanged;
}

bool Potentiometer::isValWasChangedLeft() 
{
    return valWasChangedLeft;
}

bool Potentiometer::isValWasChangedRight() 
{
    return valWasChangedRight;
}

int Potentiometer::getPrevState() 
{
    return m_prevState;
}

#endif