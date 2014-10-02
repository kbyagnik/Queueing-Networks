#include <stdlib.h>
#include<stdio.h>
#include <math.h>
#include<iostream>
using namespace std;
float gen_exp(float lambda){ 				// generating a random time with a given rate
	float p = ((1.0*rand())/RAND_MAX);
	p= (1/lambda) * log(1/(1-p));
	return p;
};
class event						//class event
{
	public:
	int qno;
	float t;
};

event min(event e1,event e2,event e3)			//finding the minimum time for an event
{
	if (e1.t<e2.t && e1.t<e3.t)
		return e1;
	else if (e3.t<e2.t && e3.t<e1.t)
		return e3;
	else
		return e2;
};

class heap						//class of a heap
{
	public:
	event h[100000];
	int n;
	
	heap(){						//constructor
	n=1;
	int i=0;
	for (i=0;i<10000;i++) h[i].t=5000000;		//in case of just 1 child.. making it minimum
	}

	void insert(event e)
	{
		h[n]=e;
		int i=n;
		while(i>1)
		{				//BUBBLING UP
			if(h[i].t<h[i/2].t)
			{
				event temp;
				temp=h[i];
				h[i]=h[i/2];
				h[i/2]=temp;
				i/=2;
			}
			else break;
		}
		n++;
	};
	
	event ext_min()					//extracting the minimum from the heap of events
	{
		event ret=h[1];
		h[1]=h[--n];
		h[n].t=5000000;
		int i=1;
		while(2*i+1<=n)				//SHIFTING DOWN
		{
			
			event mini=min(h[i],h[2*i],h[2*i+1]);
			
			if(mini.t==h[i].t)
				break;
			else if (mini.t==h[2*i].t)
			{
				event temp=h[i];
				h[i]=h[2*i];
				h[2*i]=temp;
				i=2*i;				
			}
			else
			{
				event temp=h[i];
				h[i]=h[2*i+1];
				h[2*i+1]=temp;
				i=2*i+1;
			}

		}
		return ret;
	}

};

class cust						//class for the customer
{
	public:
	float tentry,texit,id;
	cust(){
		tentry=texit=0.0;
		static int i=0;
		id=i++;
	}
};

class qnode						//qnode for class que
{
	public:
	cust *c;
	qnode *next;
	qnode()
	{
	next=NULL;
	};
};

class que
{
	public:	
	int nlinks,id;
	qnode *front,*rear;
	int out[10000];
	float prob[10000],rate;

	que(){
	static int i=0;
	id=i++;
	rear=NULL;
	front=NULL;
	}

	void enq(qnode *a)				//enqueing the customer
	{
	
		if(front==NULL){
			front=a;
			rear=a;
			rear->next=NULL;
		}
		else{
			rear->next=a;
			rear=a;
			rear->next=NULL;
		}


	}

	qnode* deq(){					//dequeing the customer
		qnode *temp;
		temp=front;
		front=front->next;
		return temp;
	}

};

int qdecide(que q)					//deciding which que it goes after deqing from the served que
{
	float sum=0,p = ((1.0*rand())/RAND_MAX);
	int i=0;	
	for(i=0;i<q.nlinks;++i)
	{
		sum+=q.prob[i];
		if(p<=sum)
		return q.out[i];
	}
	return q.out[i-1];
}

int main ()
{
	int i,j,k,totcust=0;
	float T_tot,T,t,avg_time;
	int m,n;
	float SUM=0.0;
	
	cin>>T_tot;
	cin>>m>>n;
	
	que q[m];
	for(i=0;i<m-1;i++)	
		cin>>q[i].rate;
	
	for(i=0;i<m-1;i++)
		{
		int l;
		cin>>q[i].nlinks;
		for (j=0;j<q[i].nlinks;j++)
		{
			cin>>q[i].out[j];
			cin>>q[i].prob[j];
		}
		}
	
	heap h;
	event e1,e2;
	cust *ctemp;
	qnode *qntemp=new qnode();
	
	for(i=0;i<n;i++)					//creating a heap of events in input queue
	{
		e1.t=gen_exp(q[i].rate);
		e1.qno=i;
		h.insert(e1);
	}
	T=0;
	while(T<T_tot)
	{	
		e2=h.ext_min();					//extracting min event
		i=e2.qno;
		T=e2.t;
		if(i<n)						//if i is an input queue
		{	
		
			ctemp = new cust;
			ctemp->tentry=T;
			qnode *node=new qnode;
			node->c=ctemp;

			q[i].enq(node);

			t=T+gen_exp(q[i].rate);
			e1.t=t;
			e1.qno=i;
			h.insert(e1);

			int a=qdecide(q[i]);

			if(q[a].front==NULL)
			{
				qnode *node1=new qnode;		
				node1= q[i].deq();
				q[a].enq(node1);
				t=T+gen_exp(q[a].rate);

				e1.t=t;
				e1.qno=a;	
				h.insert(e1);
		
			}
			else
			{
				qnode *node2=new qnode;
				node2=q[i].deq();
				q[a].enq(node2);
			}
	
		}
	


		else						//if it is intermediate que
		{	
			qntemp=q[i].deq();
			int a=qdecide(q[i]);

			if(q[i].front!=NULL)
			{
				t=T+gen_exp(q[i].rate);

				e1.t=t;
				e1.qno=i;
				h.insert(e1);
			}	
			
			if (a==m-1)			//output queue
			{
				qntemp->c->texit=T;
				SUM+=(T - qntemp->c->tentry );		//adding the waiting times of the customers

				totcust++;				//incrementing the total customers
				delete qntemp->c;
				delete qntemp;	
			}
			else					//interior queue
			{
				if (q[a].front==NULL)
				{	
					q[a].enq(qntemp);
					t=T+gen_exp(q[a].rate);
				
					e1.t=t;
					e1.qno=a;
					h.insert(e1);
				}
				else q[a].enq(qntemp);
			}

			

		}				

	}				//while ends
	
	
	avg_time=SUM/totcust;
	cout<<avg_time<<endl;

	
	return 0;
}
