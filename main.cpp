#include <iostream>
#include <queue>
#include <list>
#include <algorithm>
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include <xlocale.h>
const int maxAmpl=pow(2,16);
const int defAmpl=maxAmpl/10;
double translate(const double& Min, const double& Max, const double& toMin, const double& toMax, const double& value){
    return ((value-Min)/(Max-Min))*(toMax-toMin)+toMin;
}
long unsigned int randn(const long& n){
    if(n>LONG_MAX || n<=0)
        throw std::out_of_range("Rand value out of range");

    const long bucket=LONG_MAX/n;
    long unsigned int r;
    const static int intbit =sizeof(int) *8;
    const static int longbit=sizeof(long)*8;
    const static int dif    =longbit-intbit;
    int curDif=dif;
    do{
        long unsigned int random=std::rand();
        unsigned int* ptr  =reinterpret_cast<unsigned int*>(&random);

        int i=1;
        while(curDif>intbit)
        {
            int t=rand();
            memcpy(ptr+i,&t,1);
            curDif-=intbit;
            i++;
        }
        random<<=curDif;
        random|=std::rand()%(int)pow(2,curDif);

        r = random / bucket;
    }while (r>=n);
    return r;
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
    std::locale::global(std::locale("en_US.UTF-8"));
    std::wcout.imbue(std::locale());


    sf::ContextSettings settings;
    settings.antialiasingLevel=8;
    sf::VideoMode video=sf::VideoMode::getFullscreenModes()[0];
    int width=video.width=video.width/2;
    int height=video.height=video.height/2;
    sf::RenderWindow window(video,"Piano",sf::Style::Close,settings);
    window.setFramerateLimit(60);
    sf::Event event;
    sf::Font font;
    font.loadFromFile("font.otf");

    double offset=0.1;
    double upoffset=0.5;

    sf::RenderTexture innerWindow;
    innerWindow.create(width*(1-2*offset),height*(1-upoffset-offset));
    sf::Sprite innerWindowRendering;
    innerWindowRendering.setPosition(width*offset,height*upoffset);

    sf::RenderTexture text;
    text.create(width*(1-2*offset),height/**(1-upoffset-2*offset)*/);
    sf::View view=text.getView();
    view.setSize(text.getSize().x,text.getSize().y*(1-upoffset-2*offset));
    view.setCenter(view.getSize()/2.f);
    view.setViewport(sf::Rect<float>(0.f,0.f,1.f,view.getSize().y/text.getSize().y));
    text.setView(view);
    sf::Sprite textRendering;
    textRendering.setPosition(width*offset,height*offset);



    sf::RectangleShape innerWindowEdge(sf::Vector2f(innerWindow.getSize().x,innerWindow.getSize().y));
    innerWindowEdge.setPosition(innerWindowRendering.getPosition());
    innerWindowEdge.setFillColor(sf::Color::Transparent);
    innerWindowEdge.setOutlineThickness(innerWindowEdge.getSize().x/150);
    innerWindowEdge.setOutlineColor(sf::Color::White);


    sf::Text test("Delo sdelano1",font);
    sf::Text test1("Delo sdelano2",font);
    test1.setPosition(0,test.getPosition().y+(test.getGlobalBounds().height+10));
    sf::Text test2("Delo sdelano3",font);
    test2.setPosition(0,test1.getPosition().y+(test1.getGlobalBounds().height+10));
    sf::Text test3("Delo sdelano4",font);
    test3.setPosition(0,test2.getPosition().y+(test2.getGlobalBounds().height+10));


    bool stopOnRelease= true;
    bool mixed=true;

    sf::SoundBuffer buff;
    std::multimap<sf::Keyboard::Key,sf::Sound> soundQueue;

    int vamplitude=maxAmpl;
    while(window.isOpen()) {
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::TextEntered:
                    break;
                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::RBracket:
                            break;
                        case sf::Keyboard::LBracket:
                            break;
                        case sf::Keyboard::Up:
                        {
                            view.move(0,-10);
                            text.setView(view);
                            break;
                        }
                        case sf::Keyboard::Down: {
//                            if(view.getCenter().y+view.getSize().y/2<text.getSize().y) {
                                view.move(0, 10);
                                text.setView(view);
//                            }else if()
                            break;
                        }
                        default:
                            if(event.key.code<sf::Keyboard::Escape) {
                                std::vector<int16_t> buffer(44100);
                                sampleSound(buffer, 29 + event.key.code);
                                buff.loadFromSamples(buffer.data(), 44100, 1, 44100);
                                soundQueue.emplace(event.key.code, buff);
                                (--soundQueue.end())->second.play();
                            }
                            break;
                    }
                    break;
                case sf::Event::KeyReleased:
                    if(stopOnRelease)
                        switch (event.key.code) {
                            default:
                                if(event.key.code<sf::Keyboard::Escape) {
                                    soundQueue.find(event.key.code)->second.stop();
                                }
                                break;
                        }
                    break;
            }
        }

        std::erase_if(soundQueue,[](const auto& pair){return pair.second.getStatus()==sf::Sound::Stopped;});

        sf::VertexArray wave(sf::PrimitiveType::LineStrip, innerWindowEdge.getSize().x*2);
        if (!soundQueue.empty()){
            if(mixed)
            {
                for (int i = 0; i < wave.getVertexCount(); ++i) {
                    wave[i].position.x =(float) innerWindow.getSize().x / (float) wave.getVertexCount() * (float) i;
                    wave[i].position.y =(float) innerWindow.getSize().y / 2.f;

                }
                for(const auto& node:soundQueue)
                {
                    const sf::Sound& sound=node.second;
                    const int16_t* toDraw = sound.getBuffer()->getSamples();


                    for (int i = 0; i < wave.getVertexCount(); ++i) {
                        wave[i].position.y +=       (float) translate(-vamplitude, vamplitude,
                                                              -(double)innerWindow.getSize().y / 2.,
                                                              (double)innerWindow.getSize().y / 2.,
                                                              toDraw[sound.getBuffer()->getSampleCount() /
                                                                     wave.getVertexCount() * i]);

                    }
                }
            }else {
//                const sf::Sound& playing =;
//                const int16_t* toDraw = playing.getBuffer()->getSamples();
//
//                for (int i = 0; i < wave.getVertexCount(); ++i) {
//                    wave[i].position =
//                            sf::Vector2f(innerWindowEdge.getSize().x / (float) wave.getVertexCount() * (float) i,
//                                         innerWindowEdge.getSize().y / 2 +
//                                         (float) translate(-maxAmpl, maxAmpl,
//                                                           -innerWindowEdge.getSize().y / 2,
//                                                           innerWindowEdge.getSize().y / 2,
//                                                           toDraw[playing.getBuffer()->getSampleCount() /
//                                                                  wave.getVertexCount() * i])) +
//                            innerWindowEdge.getPosition();
//                }
            }
        }
        window.clear();

        text.clear();

        text.draw(test);
        text.draw(test1);
        text.draw(test2);
        text.draw(test3);

        text.display();
        textRendering.setTextureRect(text.getViewport(text.getView()));
        textRendering.setTexture(text.getTexture());
        window.draw(textRendering);
        //**//
        innerWindow.clear();

        innerWindow.draw(wave);

        innerWindow.display();
        innerWindowRendering.setTexture(innerWindow.getTexture());
        window.draw(innerWindowRendering);
        window.draw(innerWindowEdge);
        //**//
        window.display();
    }
    return 0;
}
