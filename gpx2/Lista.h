/* *********************************************************************\
 *   List Class															*		
 * 	 From book:															*
 * 	 N. Ziviani, Projeto de algoritmos, 2a ed. Thomson, 2004.			*
\* *********************************************************************/
#ifndef LISTA_H_
#define LISTA_H_
#include <stdexcept> 
using std::logic_error;
#include<iostream>
using std::cout;
using std::endl;
namespace cap3_autoreferencia {
  // @{\it Para utilizar a classe Lista<T> o tipo de dado fornecido no}@ 
  // @{\it lugar do par\^ametro de tipo T deve possuir um construtor de}@ 
  // @{\it c\'opia e os operadores <<, ==, !=, e = sobrecarregados.}@
	template <class T> class Lista {
	private:
	  class Celula {
		friend class Lista<T>;
		private:
		  T *item;
	    Celula *prox;
		  Celula () { item = 0; prox = 0; }
		  ~Celula () { if (item != 0) delete item; }
		};
	  Celula *primeiro, *ultimo, *pos;
	public:
		Lista (); // @{\it Cria uma Lista vazia}@
		T *pesquisa (const T& chave) const;
		void insere (const T& x);  
		// @{\it Insere antes do primeiro item da lista}@
	  void inserePrimeiro (T& item);
		T *retira (const T& chave) throw ( logic_error );
		T *retiraPrimeiro () throw ( logic_error );
    T *_primeiro ();
    T *proximo ();
		bool vazia () const;
		void imprime () const;
		~Lista ();
	};	
	template <class T> Lista<T>::Lista () {
	  this->primeiro = new Celula (); this->pos = this->primeiro;
	  this->ultimo = this->primeiro; this->primeiro->prox = 0;
	}	
	template <class T>	
	void Lista<T>::insere (const T& x) {  
	  this->ultimo->prox = new Celula (); 
	  this->ultimo = this->ultimo->prox; 
	  this->ultimo->item = new T (x); this->ultimo->prox = 0;
	}
	// @{\it Insere antes do primeiro item da lista}@
	template <class T>	
  void Lista<T>::inserePrimeiro (T& item) {
    Celula *aux =  this->primeiro->prox;
    this->primeiro->prox = new Celula ();
    this->primeiro->prox->item = new T(item);
    this->primeiro->prox->prox = aux;
  }
	
	template <class T>	
	T *Lista<T>::pesquisa (const T& chave) const {
	  if (this->vazia ()) return 0;
	  Celula *aux = this->primeiro;
	  while (aux->prox != 0) {
	    if (*(aux->prox->item) == chave) return aux->prox->item;
	    aux = aux->prox;
	  }
	  return 0;
	}	
	template <class T>	
	T *Lista<T>::retira (const T& chave) throw ( logic_error ) {
	  if (this->vazia ()) throw logic_error ("Erro: A lista esta vazia");
	  Celula *aux = this->primeiro;
	  while (aux->prox != 0 && *(aux->prox->item) != chave) aux=aux->prox;
	  if (aux->prox == 0) return 0; 
	  Celula *q = aux->prox;
	  T *item = q->item; aux->prox = q->prox;
	  q->item = 0; // @{\it transfere a posse da mem\'oria}@
	  if (aux->prox == 0) this->ultimo = aux;
	  delete q; return item;
	}		
	template <class T>	
	T *Lista<T>::retiraPrimeiro () throw ( logic_error ) {
	  if (this->vazia ()) throw logic_error ("Erro: A lista esta vazia");
	  Celula *aux = this->primeiro;
	  Celula *q = aux->prox;
	  T *item = q->item; aux->prox = q->prox;
	  q->item = 0; // @{\it transfere a posse da mem\'oria}@    
	  if (aux->prox == 0) this->ultimo = aux;
	  delete q; return item;
	}
  template <class T> T *Lista<T>::_primeiro () {
    this->pos = this->primeiro; 
    return this->proximo ();
  }
  template <class T> T *Lista<T>::proximo () {
    this->pos = this->pos->prox;
    if (this->pos == NULL) return NULL; 
    else return this->pos->item;
  }
	template <class T>
	bool Lista<T>::vazia () const { 
		return (this->primeiro == this->ultimo);
	}	
	template <class T>	
	void Lista<T>::imprime () const {
		Celula *aux = this->primeiro->prox;
	  while (aux != 0) { cout << *(aux->item) << endl; aux = aux->prox; }
	}	  
	template <class T> Lista<T>::~Lista () {
	  Celula *aux = this->primeiro;
	  while (aux != 0) {
	  	this->primeiro = this->primeiro->prox;
	  	delete aux; aux = this->primeiro;
	  }
	}
}
#endif 
