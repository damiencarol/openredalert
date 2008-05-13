#ifndef TALKBACK_H
#define TALKBACK_H

#include <string>
#include <map>
#include <vector>

#include "game/TalkbackType.h"

class INIFile;

using std::string;
using std::map;
using std::vector;


class Talkback
{
public:
    Talkback();
    void load(string talkback, INIFile* tbini);
    const char* getRandTalk(TalkbackType type);
    
private:
    Talkback(const Talkback&);
    Talkback& operator=(const Talkback&);

    static map<string, TalkbackType> talktype;
    static bool talktype_init;

    typedef map<TalkbackType, vector<string> > t_talkstore;
    t_talkstore talkstore;

    void merge(Talkback *mergee);
    vector<string>& getTypeVector(TalkbackType type);
    TalkbackType getTypeNum(string name);
};

#endif //TALKBACK_H
