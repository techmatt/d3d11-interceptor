
#include "main.h"

void ObjectCollection::save(const string &filename) const
{
    ofstream file(filename);

    file << objects << endl;
}

void ObjectCollection::load(const string &filename)
{
    ifstream file(filename);

    file >> objects;
}