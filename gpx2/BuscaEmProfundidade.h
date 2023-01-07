/* *********************************************************************\
 *   Graph Deep First Search											*
 * 	 From book:															*
 * 	 N. Ziviani, Projeto de algoritmos, 2a ed., Thomson, 2004.			*
 *   Note: modified in order to find the connected components			*
\* *********************************************************************/
#ifndef BUSCAEMPROFUNDIDADE_H_
#define BUSCAEMPROFUNDIDADE_H_
#include "Grafo.h" // @{\it vide Programa~\ref{c_7.4}}@
#include <iostream>
using std::cout;
using std::endl;
using cap7_listaadj_autoreferencia::Grafo; // @{\it vide Programa~\ref{c_7.4}@
namespace cap7 {
	class BuscaEmProfundidade {
  public: 
    static const unsigned char branco;
    static const unsigned char cinza;
    static const unsigned char preto;
  private: 
    int *d, *t, *antecessor;
    Grafo *grafo;
    int visitaDfs (int u, int tempo, unsigned char *cor) const;
    int visitaDfsCC (int u, int tempo, unsigned char *cor, int *vector_comp, int componente) const;
  public:
	  BuscaEmProfundidade (Grafo *grafo);
	  void buscaEmProfundidade () const;
	  void compCon (int *vector_comp) const;
	  int _d (int v) const;
	  int _t (int v) const;
	  int _antecessor (int v) const;
	  ~BuscaEmProfundidade ();
	};
	const unsigned char BuscaEmProfundidade::branco = 0;
	const unsigned char BuscaEmProfundidade::cinza  = 1;
	const unsigned char BuscaEmProfundidade::preto  = 2;
  int BuscaEmProfundidade::visitaDfs (int u, int tempo, 
                                      unsigned char *cor) const {
    cor[u] = cinza; this->d[u] = ++tempo;
//    cout << "Visita " << u << " Descoberta:" << this->d[u] << " cinza" << endl;
    if (!this->grafo->listaAdjVazia (u)) {
      Grafo::Aresta *a = this->grafo->primeiroListaAdj (u);
      while (a != NULL) {
        int v = a->_v2 ();
        if (cor[v] == branco) {
          this->antecessor[v] = u;
          tempo = this->visitaDfs (v, tempo, cor);
        }
        delete a; a = this->grafo->proxAdj (u);
      }
    }
    cor[u] = preto; this->t[u] = ++tempo;
//    cout << "Visita " << u << " Termino:" << this->t[u] << " preto" << endl;
    return tempo;
  }
  int BuscaEmProfundidade::visitaDfsCC (int u, int tempo, 
                                      unsigned char *cor, int *vector_comp, int componente) const {
    vector_comp[u]=componente;
	cor[u] = cinza; this->d[u] = ++tempo;
//    cout << "Visita " << u << " Descoberta:" << this->d[u] << " cinza" << endl;
    if (!this->grafo->listaAdjVazia (u)) {
      Grafo::Aresta *a = this->grafo->primeiroListaAdj (u);
      while (a != NULL) {
        int v = a->_v2 ();
        if (cor[v] == branco) {
          this->antecessor[v] = u;
          tempo = this->visitaDfsCC (v, tempo, cor,vector_comp,componente);
        }
        delete a; a = this->grafo->proxAdj (u);
      }
    }
    cor[u] = preto; this->t[u] = ++tempo;
//    cout << "Visita " << u << " Termino:" << this->t[u] << " preto" << endl;
    return tempo;
  }
  BuscaEmProfundidade::BuscaEmProfundidade (Grafo *grafo) {
    this->grafo = grafo; 
    int n = this->grafo->_numVertices ();
    d = new int[n]; t = new int[n]; antecessor = new int[n];
  }
  void BuscaEmProfundidade::buscaEmProfundidade () const {
    int tempo = 0; 
    unsigned char *cor = new unsigned char[this->grafo->_numVertices ()]; 
    for (int u = 0; u < grafo->_numVertices (); u++) {
      cor[u] = branco; this->antecessor[u] = -1;

    }     
    for (int u = 0; u < grafo->_numVertices (); u++)
      if (cor[u] == branco) tempo = this->visitaDfs (u, tempo, cor);
    delete [] cor;
  }
  void BuscaEmProfundidade::compCon (int *vector_comp) const {
    int tempo = 0; 
    int componente=0;
    unsigned char *cor = new unsigned char[this->grafo->_numVertices ()]; 
    for (int u = 0; u < grafo->_numVertices (); u++) {
      cor[u] = branco; this->antecessor[u] = -1;
    }     
    for (int u = 0; u < grafo->_numVertices (); u++){
      if (cor[u] == branco) {
	  	tempo = this->visitaDfsCC (u, tempo, cor, vector_comp, componente);
	  	componente++;
	  }
	}
    delete [] cor;
    delete [] this->d;
    delete [] this->t; 
	delete [] this->antecessor;
  }
  int BuscaEmProfundidade::_d (int v) const { return this->d[v]; }
  int BuscaEmProfundidade::_t (int v) const { return this->t[v]; }
  int BuscaEmProfundidade::_antecessor (int v) const { 
  	return this->antecessor[v]; 
  }
  BuscaEmProfundidade::~BuscaEmProfundidade () {
    this->grafo = NULL; 
	//delete [] this->d;
    //delete [] this->t; 
	//delete [] this->antecessor;
  }  
}
#endif
