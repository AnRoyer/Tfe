#ifndef IO_INCLUDED
#define IO_INCLUDED

void writeModels(std::vector<GModel*> &models);
void writeProFile(GModel* m);
std::vector<int> getNumFromString(std::string name);

#endif /* IO_INCLUDED */
