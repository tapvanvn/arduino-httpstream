///example of using HttpStream by Duy Nguyen
/// First, you must decide the number of headers that you would use.
/// Then you must sure that them hash (SimpleHashTable<>::hash(key)) of the headers are not duplicate. 
/// Because HttpStrem using SimpleHashTable to store key value pair of headers 
/// so if there are some duplicate of hash of keys then the corresponding headers will be stored 
/// in the same location.

#include <HttpStream.hpp>

//define a HttpStream with 2 headers and 256 of buffer size.
typedef HttpStream<2, 256> SimpleHttpStream;

SimpleHttpStream _stream;


byte mac[] =
{
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

static bool _ready = true;

EthernetServer server(80);

void setup()
{
    Serial.begin(9600);
    while(!Serial)
    {
        ;
    }

	_ready = Ethernet.begin(mac);
	
	if(_ready)
    {
		Serial.print("server begin at:");
        Serial.println(Ethernet.localIP());
	}
}

void loop()
{
	if(!_ready) return;
	
	EthernetClient client = server.available();

    if(client)
    {
		Serial.print("serve client\n");
		
		bool blank = true;
		
        while(client.connected())
        {
            if(client.available())
            {
                char c = client.read();

                _stream + c;

                //Serial.print(c);
                if (c == '\n' && blank)
                {
                    break;
                }

                blank = (c == '\n' || c == '\r');
            }
        }
		
        //working with request
        SimpleHttpStream::Value* msg = _stream.getHeader("msg");
        SimpleHttpStream::Value* phone = _stream.getHeader("phone");
		
		if(phone && msg)
        {
#if SERIAL_DEBUG
            Serial.print("send:");
            Serial.println(msg->ToString());
            Serial.print("to:");
            Serial.println(phone->ToString());
#endif
            //SendSMS(phone->ToString(), msg->ToString());

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            client.println("{status:true}");
        }
        else
        {
            Serial.println("something went wrong!!!");
        }

        delay(1);
        client.stop();
        _stream.reset();
	}
}