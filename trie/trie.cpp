#include <iostream>
  #include <thread>
  #include <unistd.h>
  #include <cstdio>
  #include "trie.hpp"
  #pragma once
  /////////////////////////////////////////
  #define ASCII_CHAR(C) ((int)C - (int)'a')
  #define def 10
  #define ERAN 0
  ////////////////////////////////////
extern pthread_mutex_t _eransMutex;
///////////////////////////////
  Trie::Trie()
  {
    root=new Node;
    root->isLeaf=true;
    wordCount=0;
    read_size=write_size=del_size=def;
    read_count=write_count=del_count=0;
  }
  ////////////////////////////////////
  void Trie::insert(string str,string url)
  {
      pthread_mutex_lock(&_eransMutex);
    if(root==NULL)
    {
      write_count--;
      return ;
    }
      Node* t=root;
      size_t length=str.length();
      for (size_t i = 0; i < length; i++)
       {
        int index=ASCII_CHAR(str.at(i));
        if(t->isLeaf)
        {
          t->isLeaf=false;
          t->children[index]=new Node;
        }
        if(t->children[index]==NULL)t->children[index]=new Node;
        t=t->children[index];
      }
      if(!(t->isWord))
      {
      t->file.open("../trie/archive/"+str+".txt");
      t->file_name="../trie/archive/"+str+".txt";
      t->isWord=true;
      wordCount++;
      }
      t->file<<url<<endl;
      write_count--;
      pthread_mutex_unlock(&_eransMutex);
  }
  ////////////////////////////////////
Node* Trie::find(string str)
  {
  if(root==NULL) return NULL;
  Node* t=new Node;
  t=root;
  size_t length=str.length();

  wod.lock();
  for (size_t i = 0; i < length; i++)
   {
    int index=ASCII_CHAR(str.at(i));
    // while(t->delworkingHere || t->writeworkingHere){}//std::cout << "waiting on find" << '\n';}
    // t->delworkingHere=true;
    if(t==NULL || t->isLeaf || t->children[index]==NULL)
    {
      t->delworkingHere=false;
      return NULL;
    }
    t->delworkingHere=false;
    t=t->children[index];
  }
  wod.unlock();
  return t;
}
  ////////////////////////////////////
  void Trie::del(string str)
  {
    Node* n=find(str);
    if(n==NULL)
    {
    del_count--;
    return /*false*/;
    }
    if(n->isWord)
    {
    std::cout <<"removing "<< n->file_name << '\n';
    //n->file.close();
    remove(n->file_name.c_str());
    n->isWord=false;
    wordCount--;
    del_count--;
    return /*true*/;
    }
    del_count--;
    return /*false*/;
  }
  ////////////////////////////////////
  Node* Trie::search(string str)
  {
    Node* n=find(str);
    if(n==NULL || (n!=NULL && n->isWord==false))
    {
      read_count--;
      std::cout << "ERR_404"<< '\n';
      return NULL;
    }
    read_count--;
    std::cout <<str<< '\n';
    return n;
  }
  ///////////////////////////////////
  void Trie::go(SYNC type, string str,string url)
  {
    switch(type)
    {

          case READ:
          {
            std::thread (&Trie::search,this,str).detach();
              read_count++;
              return;
          }

    case WRITE:
            {
              if(url=="NULL")
              {
                std::cout << "invalid url" << '\n';
                return;
              }
                insert(str,url);
                return;
            }

          case DELETE:
          {
              std::thread (&Trie::del,this,str).detach();
              del_count++;
              return;
          }
          default:
          return;
    }
}
  ////////////////////////////////////
    void Trie::setSize(SYNC type, size_t size)
    {
      switch(type)
      {

          case READ:
          {
              read_count=size;
          }

          case WRITE:
          {
              write_count=size;
          }

          case DELETE:
          {
              del_count=size;
          }
      }
   }
/////////////////////////////////////////////
