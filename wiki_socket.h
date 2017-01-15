#ifndef WIKI_SOCKET_H
#define WIKI_SOCKET_H

int  openWikiConnection();
void closeWikiConnection();

void ShowCerts();

int fetchWikiPage(const char* search_term);

int readFragmentOfWikiPage(char* buffer, const unsigned int buffSize);

void closeSocket();
void freeTCPResources();

#endif