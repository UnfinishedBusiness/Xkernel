#ifndef STATISTICS_H
#define STATISTICS_H

class Statistics{
    public:
        void init();
        void render();
        void open();
        void close();
    private:
        bool isOpen;
};

#endif