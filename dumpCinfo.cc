#include <stdio.h>
#include <iostream>
#include <string>
#include <pcrecpp.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include <sys/file.h>
#include <sys/errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define BIT(n)       (1ULL << (n))

class Rec
{
public:
    struct AStat {
       time_t DetachTime;
       long long BytesDisk;
       long long BytesRam;
       long long BytesMissed;

       void dump() { 
char s[1000];
struct tm * p = localtime(&DetachTime);

strftime(s, 1000, "%c", p);
//strftime(s, 1000, "%A, %B %d %Y", p);


          const time_t* t = &DetachTime;
          printf("[%s], bytesDisk=%lld,  bytesRAM=%lld, bytesMissed=%lld\n", s, BytesDisk, BytesRam, BytesMissed);
       }
    };

public:
    int        m_version;
    long long  m_bufferSize;
    int        m_sizeInBits;
    char*      m_buff;
    int        m_fileBlock;
    int        m_accessCnt;
    std::vector<AStat> m_stat;

    bool   m_complete; //cached

   Rec(): m_version(-1),
          m_bufferSize(1024*1024),
          m_buff(0), m_sizeInBits(0),
          m_fileBlock(-1),
          m_accessCnt(0),
          m_complete(false) 
   {
      struct stat st;
   }

    ~Rec() {
        if (m_buff) delete [] m_buff;
    }

    void setBit(int i)
    {
        int cn = i/8;
        int off = i - cn*8;
        m_buff[cn] |= BIT(off);
    }

    bool testBit(int i) const
    {
        int cn = i/8;
        int off = i - cn*8;
        return (m_buff[cn] & BIT(off)) == BIT(off);
    }
    void resizeBits(int s)
    {
       //printf("num BLOCKS = %d \n", s);
        m_sizeInBits = s;
        m_buff = (char*)malloc(getSizeInBytes());
        memset(m_buff, 0, getSizeInBytes());
    }

    int getSizeInBytes() const
    {
        return (m_sizeInBits-1)/8 + 1;
    }

    int getSizeInBits() const
    {
        return m_sizeInBits;
    }

    bool isComplete() const {
        return m_complete;
    }

    void setComplete(int c) {
        m_complete = c;
    }
   

    //______________________________________________________________________________

    //#include <assert.h>
    int read(FILE* fp)
   {
      int fl = flock(fileno(fp),LOCK_SH );
      if (fl) printf("read lock err %s \n", strerror(errno));
      
      int off = 0;

      off += fread(&m_version, sizeof(int), 1, fp);
      off += fread(&m_bufferSize, sizeof(long long), 1, fp);
      int sb;
      if (fread(&sb, sizeof(int), 1, fp) != 1) return -1;
      resizeBits(sb);
      fread(m_buff, getSizeInBytes() , 1, fp);
 
      off = fread(&m_accessCnt, sizeof(int), 1, fp);

      m_stat.resize(m_accessCnt);
      AStat stat;
      int ai = 0;
      for (std::vector<AStat>::iterator i = m_stat.begin(); i != m_stat.end(); ++i, ai++) {
         int off = fread( &(*i), sizeof(AStat), 1, fp);
         if(off !=  1)
         { 
            printf("AStat[%d] read error, AStat size[%d] read returned %d \n",ai, sizeof(AStat), (int)off );
            return -1;
         }
      }

      int flu = flock(fileno(fp),LOCK_UN);
      if (flu) printf("read un-lock err %s \n", strerror(errno));

      m_complete = isAnythingEmptyInRng(0, sb) ? false : true;
      return 1;
   }
    //______________________________________________________________________________


    int write(FILE* fp)
    {
        if (fwrite(&m_version, sizeof(int), 1, fp) != 1) return -1;
        if (fwrite(&m_bufferSize, sizeof(long long), 1, fp) != 1) return -1;
        int nb = getSizeInBits();
        fwrite(&nb, sizeof(int), 1, fp); 
        fwrite(m_buff, getSizeInBytes(), 1, fp);

        for (std::vector<AStat>::iterator i = m_stat.begin(); i != m_stat.end(); ++i) {
            fwrite( &(*i), sizeof(AStat), 1, fp);
        }
    }
    //______________________________________________________________________________



    bool isAnythingEmptyInRng(int firstIdx, int lastIdx)
    {
        for (int i = firstIdx; i <= lastIdx; ++i)
            if(! testBit(i)) return true;
    }
    //______________________________________________________________________________


   void print(bool full)
   {
      bool printBits = false;
      int cntd = 0;
      if (printBits) printf("printing  %d blocks: \n", m_sizeInBits);
      for (int i = 0; i < m_sizeInBits; ++i)
      {
         if (printBits) printf("%d ", testBit(i));
         if (testBit(i)) cntd++;

      }
      if (printBits) printf("\n");

      printf("Stat version == %d,  bufferSize %lld nBlocks %d nDownlaoded %d %s\n",m_version, m_bufferSize, m_sizeInBits , cntd, (m_sizeInBits == cntd) ? " complete" :"");
      if (m_fileBlock >=0 )
      {
         printf("FileBlock %d", m_fileBlock);
      }
      if (full) {
         // printf("num access %d \n", m_accessCnt);
         printf("\n");
         for (int i=0; i < m_accessCnt; ++i)
         {  
            printf("access %d >>  ", i);
            m_stat[i].dump();
      
         }
      }
   }

};

int main(int argc, char* argv[])
{
   bool fullprint = false;
   if (argc > 2) {
      fullprint = true;
   }

   /*
     {
     Rec r;
     FILE* f = fopen(argv[1],"w+");
     r.resizeBits(19);
     r.setBit(0);
     r.setBit(1);
     r.setBit(2);
     r.setBit(7);
     for(int i = 8; i < 18; ++i) {
     r.setBit(8+i);
     }

     r.m_stat.push_back(Rec::AStat());
     r.m_stat.back().m_closeTime = time(0);
     int res = r.write(f);
     printf("writing res = %d .....\n", res);
     r.print();
     fclose(f);
     }
   */
   {
      printf("\n----------------------------------------------\n");
      printf("%s .....\n", argv[1]);

      Rec r;
      FILE* f = fopen(argv[1],"r");
      int res = r.read(f);
      fclose(f);
      pcrecpp::RE re  ( "___(\\d+)_(\\d+)\\.cinfo$" );  
      std::string name(argv[1]);
      long long bsize, boff;

      if (re.PartialMatch(name, &bsize, &boff))
      { 
         r.m_fileBlock =  boff/bsize;
         //         printf("Block %d\n", fileBlock);
      }


      r.print(res >=0);
      r.isAnythingEmptyInRng(7, 11);

   }

}
