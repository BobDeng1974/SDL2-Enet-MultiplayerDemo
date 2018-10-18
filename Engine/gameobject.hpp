#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <vector>
#include "structs.hpp"

enum GAMEOBJECT_TYPE : uint8_t { 
    GAMEOBJECT_TYPE_DEFAULT = 0 , 
    GAMEOBJECT_TYPE_INITIAL_SCENE ,  
    GAMEOBJECT_TYPE_RENDER , 
    GAMEOBJECT_TYPE_PHYSIC };

class GameObject {
    private:
        uint8_t                     obj_type;
        static int                  open_id;
    protected:
        int                         id;
        std::string                 name;
        GameObject*                 parent;
        std::vector<GameObject*>    children;
    public:
        GameObject( uint8_t _obj_type = GAMEOBJECT_TYPE_DEFAULT );
        
        uint8_t                     getObjectType(){return obj_type;}
        int                         getId(){return id;};
        std::string                 getName(){return name;};
        void                        setName( std::string _name ){name = _name;}
        void                        addChild( GameObject* child );
        void                        removeChild( GameObject* child );
        GameObject*                 getParent() const{return parent;};
        GameObject*                 getChild( std::string child_name );
        GameObject*                 getChild( int child_id );
        std::vector<GameObject*>&   getChildren(){return children;}
        bool                        isInsideTree() const ;


        virtual Vector2 getPosition() const{return Vector2(0,0);}
        virtual float   getRotation() const{return 0;}

        virtual void    frameUpdate(){};
        virtual void    enteredTree(){};
        virtual void    exitedTree(){};
         
};

inline bool                        GameObject::isInsideTree() const{
    if( obj_type == GAMEOBJECT_TYPE_INITIAL_SCENE )
        return true;
    else{
        if( getParent() == NULL )
            return false;
        else
            return getParent()->isInsideTree();   
    }
} 



#endif