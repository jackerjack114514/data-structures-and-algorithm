#include<bits/stdc++.h>
#ifdef _WIN32
#include<windows.h>
#endif
using namespace std;

/*
#include<cstddef>
#include<functional>
#include<initializer_list>
#include<iostream>
#include<stdexcept>
#include<utility>
*/


/*
<cstddef>	定义 size_t、nullptr_t、ptrdiff_t 等
<functional>	函数对象、std::function、std::hash 等
<initializer_list>	支持 {1, 2, 3} 初始化列表
<iostream>	输入输出流（cin、cout）
<stdexcept>	标准异常类（runtime_error、logic_error）
<utility>	std::pair、std::move、std::forward
*/

template<typename T>   //模板声明，令同一段代码可以适用于多种数据类型，不同类型无需复写。
class List;

template<typename T>    //链表节点类，以下简称为Node 
class ListNode{
    friend class List<T>; //调用时必须申请友元 

private:
    T data_{};
    ListNode*pred_=nullptr;
    ListNode*succ_=nullptr;

public:
    ListNode()=default;
    ListNode(const T&value,ListNode*pred=nullptr,ListNode*succ=nullptr)
        :data_(value),pred_(pred),succ_(succ){}    //节点值、前链与后链 

    T&data(){return data_;}
    const T&data()const{return data_;}

    ListNode*pred(){return pred_;}
    const ListNode*pred()const{return pred_;}

    ListNode*succ(){return succ_;}
    const ListNode*succ()const{return succ_;}

    ListNode*insertAsPred(const T&value){
        ListNode*node=new ListNode(value,pred_,this);
        pred_->succ_=node;
        pred_=node;
        return node;
    }

    ListNode*insertAsSucc(const T&value){
        ListNode*node=new ListNode(value,this,succ_);
        succ_->pred_=node;
        succ_=node;
        return node;
    }
};


template<typename T>  //双向链表 
class List{
public:
	using Node=ListNode<T>;  
    using Position=Node*;

private:
	std::size_t size_=0;
    Position header_=nullptr;
    Position trailer_=nullptr;

    void init(){
        header_=new Node;
        trailer_=new Node;
        header_->succ_=trailer_;
        trailer_->pred_=header_;
        size_=0;
    }

    void checkRank(std::size_t rank)const{
        if(rank>=size_) throw std::out_of_range("链表下标越界");
    }

    void checkRealNode(Position p)const{
        if(p==nullptr||p==header_||p==trailer_) throw std::invalid_argument("非有效数据节点"); 
    }

    Position nodeAt(std::size_t rank)const{  // POINT
        checkRank(rank);
        Position p=header_->succ_;
        while(rank-->0) p=p->succ_;
        return p;
    }

public:
    List(){init();}
    List(std::initializer_list<T>values){
        init();
        for(const T&value:values){
            insertAsLast(value);
        }
    }

    List(const List&other){
        init();
        for(Position p=other.header_->succ_;p!=other.trailer_;p=p->succ_)  insertAsLast(p->data_);
    }

    List&operator=(const List&other){
        if(this!=&other){
            List temp(other);
            swap(temp);
        }
        return*this;
    }

    ~List(){ 
        clear();
        delete header_;
        delete trailer_;
    }

    void swap(List&other)noexcept{ 
        std::swap(size_,other.size_);
        std::swap(header_,other.header_);
        std::swap(trailer_,other.trailer_);
    }



    std::size_t size()const{return size_;} //访问 
    bool empty()const{return size_==0;}

    Position first()const{return header_->succ_;}
    Position last()const{return trailer_->pred_;}

    T&operator[](std::size_t rank){return nodeAt(rank)->data_;}
    const T&operator[](std::size_t rank)const{return nodeAt(rank)->data_;}

    Position insertAsFirst(const T&value){
        ++size_;
        return header_->insertAsSucc(value);
    }

    Position insertAsLast(const T&value){
        ++size_;
        return trailer_->insertAsPred(value);
    }

    Position insertA(Position p,const T&value){  //p必须为链表内节点。此处需要添加报错error 
        checkRealNode(p);
        ++size_;
        return p->insertAsSucc(value);
    }

    Position insertA(std::size_t rank,const T&value){return insertA(nodeAt(rank),value);} 

    Position insertB(Position p,const T&value){
        checkRealNode(p);
        ++size_;
        return p->insertAsPred(value);
    }

    Position insert(std::size_t rank,const T&value){ 
        if(rank>size_) throw std::out_of_range("插入位置越界");    //rank==size()即为插入末尾 
        return(rank==size_)
			?insertAsLast(value)
            :insertB(nodeAt(rank),value);    //?：三目运算符，“？”为条件为真时return,“：”为条件为假时return。 
    }

    T remove(Position p){  //删除&清空 
        checkRealNode(p);
        p->pred_->succ_=p->succ_;
        p->succ_->pred_=p->pred_;
        T value=std::move(p->data_);
        delete p;
        --size_;
        return value;
    }

    T removeAt(std::size_t rank){return remove(nodeAt(rank));}  //定点删除 

    std::size_t clear(){
        const std::size_t oldSize=size_;
        while(!empty()) remove(first());
        return oldSize;
    }

    Position find(const T&value,std::size_t n,Position p)const{
        for(std::size_t i=0;i<n;++i){
            if(p==nullptr||p->pred_==header_) return nullptr;  //向前查找，同元素最后一个即左侧头元素 
            p=p->pred_;
            if(p->data_==value) return p;
        }
        return nullptr;
    }

    Position find(const T&value)const{ //无序链表查找
        return find(value,size_,trailer_);
    }

    Position search(const T&value,std::size_t n,Position p)const{  //升序链表向后查找，返回最后一个同元素 
        for(std::size_t i=0;i<n;++i){
            if(p==nullptr||p->pred_==header_) return nullptr;
            p=p->pred_;
            if(!(value<p->data_)) return p;
        }
        return nullptr;
    }

    Position search(const T&value)const{
        return search(value,size_,trailer_);
    }

    std::size_t disordered()const{ //相邻逆序对数量；0则链表非降序
        std::size_t count=0;
        for(Position p=first();p!=trailer_&&p->succ_!=trailer_;p=p->succ_){
            if(p->succ_->data_<p->data_) ++count;
        }
        return count;
    }

    
    template<typename Compare=std::less<T>>//稳定插入升序排序
    void sort(Compare less=Compare()){

        Position current=first()->succ_;
        while(current!=trailer_){
            Position next=current->succ_;
            Position pos=current->pred_;

            while(pos!=header_&&less(current->data_,pos->data_)) pos=pos->pred_;

            if(pos->succ_!=current){
                current->pred_->succ_=current->succ_;
                current->succ_->pred_=current->pred_;
                current->pred_=pos;
                current->succ_=pos->succ_;
                pos->succ_->pred_=current;
                pos->succ_=current;
            }
            current=next;
        }
    }


    void reverse(){
    	Position p=header_;
    	while(p!=nullptr){
    		Position next=p->succ_;
    		std::swap(p->pred_,p->succ_);
    		p=next;
		}
    	std::swap(header_,trailer_);
	}


    std::size_t deduplicate(){ //元素唯一化，O(n^2) 
        if(size_<2) return 0;

        const std::size_t oldSize=size_;
        std::size_t checked=0;
        Position p=first();

        while(p!=trailer_){
            Position next=p->succ_;
            Position duplicate=find(p->data_,checked,p);
            if(duplicate!=nullptr) remove(p);
			else checked++;
            p=next;
        }
        return oldSize-size_;
    }

    std::size_t uniquify(){   //有序链表元素唯一化，O(n) 
        if(size_<2) return 0;

        const std::size_t oldSize=size_;
        Position p=first();
        Position q=p->succ_;

        while(q!=trailer_){
            if(p->data_==q->data_){
                Position next=q->succ_;
                remove(q);
                q=next;
            }else{
                p=q;
                q=q->succ_;
            }
        }
        return oldSize-size_;
    }


    template<typename Visitor> //遍历 
    void traverse(Visitor visit){
        for(Position p=first();p!=trailer_;p=p->succ_){
            visit(p->data_);
        }
    }

    template<typename Visitor>
    void traverse(Visitor visit)const{
        for(Position p=first();p!=trailer_;p=p->succ_){
            visit(static_cast<const T&>(p->data_));
        }
    }
};


template<typename T>
void printList(const List<T>&list,const char*title){  //打印链表 
    std::cout<<title<<" [size="<<list.size()<<"]: ";
    list.traverse([](const T&value){std::cout<<value<<' ';});
    std::cout<<'\n';
}

int main(){
    SetConsoleOutputCP(936);
    List<int>list{3,1,2,2,4,3};
    printList(list,"初始链表");

    list.insertAsFirst(8);
    list.insertAsLast(5);
    list.insertA(list.first(),7);// 在首节点之后插入
    list.insertB(list.last(),6);// 在尾节点之前插入
    printList(list,"插入后");

    std::cout<<"first = "<<list.first()->data()
              <<", last = "<<list.last()->data()<<'\n';
    std::cout<<"first.succ = "<<list.first()->succ()->data()<<'\n';
    std::cout<<"逆序相邻对数量 = "<<list.disordered()<<'\n';

    if(auto p=list.find(4)){
        std::cout<<"find(4) = "<<p->data()<<'\n';
    }

    std::cout<<"无序去重删除 "<<list.deduplicate()<<" 个节点\n";
    printList(list,"无序去重后");

    list.sort();
    printList(list,"升序排序后");

    if(auto p=list.search(4)){
        std::cout<<"search(4) 返回 = "<<p->data()<<'\n';
    }

    // 加入相邻重复元素，演示有序链表去重。
    list.insertA(list.search(4),4);
    list.insertA(list.search(4),4);
    printList(list,"加入重复 4 后");
    std::cout<<"有序去重删除 "<<list.uniquify()<<" 个节点\n";
    printList(list,"有序去重后");

    std::cout<<"删除下标 0 的元素 = "<<list.removeAt(0)<<'\n';
    list.reverse();
    printList(list,"逆置后");

    // traverse() 也可以直接修改元素。
    list.traverse([](int&value){value*=10;});
    printList(list,"所有元素乘 10 后");

    return 0;
}
