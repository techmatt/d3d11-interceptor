
#include "main.h"

void ObjectCollection::save(const string &filename) const
{
    BinaryDataStreamFile file(filename, true);
    file << objects;
    file.closeStream();
}

void ObjectCollection::load(const string &filename)
{
    BinaryDataStreamFile file(filename, false);
    file >> objects;
    file.closeStream();
}
