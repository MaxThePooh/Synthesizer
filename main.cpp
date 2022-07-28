#include <iostream>
#include <queue>
#include <list>
#include <algorithm>
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
const int maxAmpl=pow(2,16);
const int defAmpl=maxAmpl/4;
double translate(const double& Min, const double& Max, const double& toMin, const double& toMax, const double& value){
    return ((value-Min)/(Max-Min))*(toMax-toMin)+toMin;
}
std::vector<int16_t>& sampleSound(std::vector<int16_t>& buff,const int& n,const int& amplitude=defAmpl,const double& expv=2,double  hertz=0)
{
    size_t size=buff.size();
    for (int i = 0; i < size; ++i) {
        if(hertz==0)
            hertz=27.5*pow(2,n/12.);
        if(expv!=0)
            buff[i]=sin(i/static_cast<double>(size)*3.14*2*hertz)*amplitude*exp(-i/44100.*expv);
        else
            buff[i]=sin(i/static_cast<double>(size)*3.14*2*hertz)*amplitude;
    }
    return buff;
}
std::vector<int16_t>& addSound(std::vector<int16_t>& buff,const int& n,const int& amplitude=defAmpl,const double& expv=2,double  hertz=0)
{
    size_t size=buff.size();
    for (int i = 0; i < size; ++i) {
        if(hertz==0)
            hertz=27.5*pow(2,n/12.);
        if(expv!=0)
            buff[i]+=sin(i/static_cast<double>(size)*3.14*2*hertz)*amplitude*exp(-i/44100.*expv);
        else
            buff[i]+=sin(i/static_cast<double>(size)*3.14*2*hertz)*amplitude;
    }
    return buff;
}

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel=8;
    sf::VideoMode video=sf::VideoMode::getFullscreenModes()[0];
    int width=video.width=video.width/2;
    int height=video.height=video.height/2;
    sf::RenderWindow window(video,"Piano",sf::Style::Close,settings);
    window.setFramerateLimit(60);
    sf::Event event;

    double offset=0.1;
    double upoffset=0.5;
    sf::Rect<int> funcWindow(width*offset,height*upoffset,
                             width*(1-2*offset),height*(1-upoffset-offset));
    sf::RectangleShape innerWindow(sf::Vector2f(funcWindow.width,funcWindow.height));
    innerWindow.setPosition(funcWindow.left,funcWindow.top);
    innerWindow.setFillColor(sf::Color::Transparent);
    innerWindow.setOutlineThickness(innerWindow.getSize().x/150);
    innerWindow.setOutlineColor(sf::Color::White);

    int amplitude=pow(2,15);
    std::vector<int16_t> newbuff(44100);
    int n=29;

    std::map<sf::Keyboard::Key,sf::Sound> soundQueue;
    std::map<sf::Keyboard::Key,sf::SoundBuffer> bufferList;
    double expv=5;
    bool stopOnRelease= false;
    const sf::Sound* last= nullptr;
    while(window.isOpen()) {
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::RBracket:
                            if(amplitude<maxAmpl/2)
                            amplitude*=2;
                            break;
                        case sf::Keyboard::LBracket:
                            if(amplitude>1)
                                amplitude/=2;
                            break;
                        case sf::Keyboard::Num1:
                        case sf::Keyboard::Num2:
                        case sf::Keyboard::Num3:
                        case sf::Keyboard::Num4:
                        case sf::Keyboard::Num5:
                        case sf::Keyboard::Num6:
                        case sf::Keyboard::Num7:
                        case sf::Keyboard::Num8:
                        case sf::Keyboard::Num9:
                            sampleSound(newbuff,n+event.key.code-26,amplitude/2,expv);
                            addSound(newbuff,n+event.key.code-25,amplitude/2,expv);
                            bufferList[event.key.code].loadFromSamples(newbuff.data(),44100,1,44100);
                            soundQueue[event.key.code].setBuffer(bufferList[event.key.code]);
                            soundQueue[event.key.code].play();
                            last=&soundQueue[event.key.code];
                            break;
                        case sf::Keyboard::Num0:
//                            sampleSound(newbuff,n+10,amplitude,expv);
                            bufferList[sf::Keyboard::Num0].loadFromFile("./knock.ogg");
                            soundQueue[sf::Keyboard::Num0].setBuffer(bufferList[sf::Keyboard::Num0]);
                            soundQueue[sf::Keyboard::Num0].play();
                            last=&soundQueue[event.key.code];

                            break;
                    }
                    break;
                case sf::Event::KeyReleased:
                    if(stopOnRelease)
                        soundQueue[event.key.code].stop();
                    break;
            }
        }
        sf::RenderTexture text;

        std::erase_if(soundQueue,[](const auto& pair){return pair.second.getStatus()==sf::Sound::Stopped;});

        sf::VertexArray wave(sf::PrimitiveType::LineStrip, innerWindow.getSize().x*2);
        if (!soundQueue.empty()){
            const sf::Sound& playing=*last;
            const int16_t* toDraw = playing.getBuffer()->getSamples();
            for (int i = 0; i < wave.getVertexCount(); ++i) {
                wave[i].position = sf::Vector2f (innerWindow.getSize().x / (float)wave.getVertexCount() * (float)i, innerWindow.getSize().y/2 +
                                    (float) translate(-maxAmpl, maxAmpl,
                                                      -innerWindow.getSize().y /2,
                                                      innerWindow.getSize().y /2,
                                                      toDraw[playing.getBuffer()->getSampleCount()/wave.getVertexCount()*i]))+innerWindow.getPosition();
            }
        }
        window.clear();
        window.draw(innerWindow);
        window.draw(sf::Sprite(text.getTexture()));
        window.draw(wave);
        window.display();
    }
    return 0;
}
