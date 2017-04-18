///HttpStream v1.0
///author: Duy Nguyen <tapvanvn@gmail.com>
///a simple analyser to analyse a http request from client.
///the main purpose of this class is detecting the headers from GET request.
///it can't detect the headers that is embeded in the body now. I hope that i can expand it soon.

#ifndef _H_HTTP_STREAM
#define _H_HTTP_STREAM

#include <stdint.h>
#include "SimpleHashTable.hpp"

#define HTTP_STREAM_BUFF_LEN 256

class HttpStream
{
public:
    enum class TokenType
    {
        METHOD, PATH, KEY, VALUE
    };

    class Value
    {
    public:
        Value(const char* val):_val(0)
        {
            uint16_t len = strlen(val);
            if(len > 0)
            {
                _val = new char[len + 1];
                memcpy(_val, val, len);
                _val[len] = '\0';
            }
        }
        ~Value()
        {
            if(_val) delete[] _val;
            _val = 0;
        }
        const char* ToString()
        {
            return (const char*) _val;
        }
    private:
        char *_val;
    };

    typedef SimpleHashTable<3, Value> KeyValuePair;

    class Token
    {
    public:
        TokenType _type;
        char* _content;
    };

    enum class State
    {
        METHOD = 0, PATH = 1, GET_KEY_VALUE = 2
    };

    HttpStream():_curr_token(0), _curr_buff(0), _state((uint8_t) State::METHOD)
    {
        _token[_curr_token]._content = &_buffer[_curr_buff];
        _headers.clear();
    }

    void reset()
    {
        _headers.deepClear();
        _curr_token = 0;
        _curr_buff= 0;
        _state = (uint8_t) State::METHOD;
        _token[_curr_token]._content = &_buffer[_curr_buff];
    }

    void operator + (char c)
    {
        if( (c >= 'a' && c <= 'z')
            || (c >= 'A' && c <= 'Z')
            || (c >= '0' && c <= '9')
            || (c == '%' || c == '+' || c == '_' || c == '.' || c == '/')
        )
        {
            _buffer[_curr_buff++] = c;
            if(_curr_buff == HTTP_STREAM_BUFF_LEN)
            {
                meaning();
                _curr_buff = 0;
                _curr_token = 0;
                _token[_curr_token]._content = &_buffer[_curr_buff];
            }
        }
        else if(c == '\n')
        {
            meaning(true);
            //cut the line
            _curr_buff = 0;
            _curr_token = 0;
            _token[_curr_token]._content = &_buffer[_curr_buff];
        }
        else
        {
            _buffer[_curr_buff++] = '\0';
            _curr_token++;
            //cut the token
            _token[_curr_token]._content = &_buffer[_curr_buff];
        }
    };

    Value* getHeader(const char* key)
    {
        return _headers.get(key);
    };


protected:
private:
    void meaning(bool end_line = false)
    {
        for(uint8_t i = 0; i< _curr_token; i++)
        {
            //ignore empty token
            if(strlen(_token[i]._content) == 0) continue;

            switch ((State) _state)
            {
                case State::METHOD:
                    if(strcmp(_token[i]._content,"GET") == 0)
                    {
                        _state++;
                    }
                    else if (strcmp(_token[i]._content,"POST") == 0)
                    {
                        _state++;
                    }
                    else
                    {
                        //do not support other method
                    }
                break;

                case State::PATH:
                    _state++;
                break;

                case State::GET_KEY_VALUE:
                    {
                        char *ptr = _token[i]._content;

                        if( strlen(ptr) >= 4 && ptr[0] == 'H' && ptr[1] == 'T' && ptr[2] == 'T' && ptr[3] == 'P')
                        {
                            _state ++;
                            return;
                        }

                        if(i < _curr_token - 1)
                        {
                            /*
                            Serial.print("key: ");
                            Serial.print(ptr);
                            Serial.print(" value:");
                            Serial.println(_token[i + 1]._content);
                            */
                            _headers.put (ptr, new Value(_token[i + 1]._content));
                            i++;
                        }

                    }
                break;

            }

        }

        if(end_line)
        {
            if(_state <= (uint8_t)State::GET_KEY_VALUE)
            {
                _state = (uint8_t)State::GET_KEY_VALUE + 1;
            }
        }
    };
    Token _token[50];
    uint8_t _curr_token;
    char _buffer[HTTP_STREAM_BUFF_LEN];
    uint16_t _curr_buff;
    uint8_t _state;
    KeyValuePair _headers;
};


#endif //_H_HTTP_STREAM
