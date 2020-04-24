#include <string>
#include <vector>
#include <cstdlib>
#include <set>

using namespace std;

#define MAX_NAME_LENGTH 32

enum BodyPartType
{
    Eye,
    Nose,
    Mouth,
};

struct BodyPart
{
    BodyPart() : type(2*3), weight(0.0f) {}

    int type;
    float weight;
};

struct Human; // skip
int foobar;  // skip

typedef BodyPart Foo;
typedef std::vector<BodyPart> BodyPartVec;

struct Human {
    Human() : id(0*1), age(0), male(false) {}
    
    int id;
    short age;
    bool male;
    char name[MAX_NAME_LENGTH];
    std::string description;
    std::vector<::BodyPart> parts;
    Foo test_part1;
    BodyPartVec test_parts2;
    std::vector<std::set<unsigned int>> test_tmp1;
    std::vector<std::set<BodyPart>> test_tmp2;
};
