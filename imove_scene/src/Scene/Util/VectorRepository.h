//
// Created by Wouter Posdijk on 17/05/16.
//

#ifndef IMOVE_VECTORREPOSITORY_H
#define IMOVE_VECTORREPOSITORY_H

#include <vector>
#include "Repository.h"
#include <algorithm>
#include <memory>
#include <assert.h>

template<typename T>
class VectorRepository : public Repository<T>
{
private:
    typename std::vector<std::shared_ptr<T>> items;
    std::vector<long> scheduledForRemoval;

public:
    void add(const std::shared_ptr<T>& item) override
    {
        assert(item);
        items.push_back(item);
    }

    void scheduleForRemoval(const std::shared_ptr<T> &item) override
    {
        typename std::vector<std::shared_ptr<T>>::iterator pos = std::find(items.begin(),items.end(),item);
        if(pos > items.end())
        {
            //TODO throw error
            return;
        }
        scheduledForRemoval.push_back(pos - items.begin());
    }

    void removeAll() override
    {
        int modifier = 0;
        for(auto &index : scheduledForRemoval)
        {
            items.erase(items.cbegin()+index+modifier);
            modifier--;
        }
        scheduledForRemoval.clear();
    }

    unsigned long size() override
    {
        return items.size();
    }

    bool has(unsigned long id) override
    {
        return id > 0 && id <= items.size();
    }

    typename std::shared_ptr<T> &get(unsigned long id) override
    {
        return items[id];
    }

    void for_each(const std::function<void(std::shared_ptr<T>)> &f) override {
        for(auto &item : items){
            f(item);
        }
    }


};


#endif //IMOVE_VECTORREPOSITORY_H