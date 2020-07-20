//Implementação da Árvore B*, aplicando fundamentos do clean code
//Autor : Cleyton de Souza Oliveira   
#include <stdio.h>
#include <stdlib.h>

#define M 4
#define DIRETORIO "paginas.txt"
int ID_DISPONIVEL = 2;

typedef struct pagina{
	int paginaInterna;
	int elementos[M];
	int quantidadeElementos;
	int filhosIds[M+1];
	int id;
	int paiId;
} PAGINA;

typedef PAGINA* ARVORE_B;



//Manipulacao do ARQUIVO
void atualizarPaginaArquivo(ARVORE_B arvore);
ARVORE_B buscarPaginaArquivo(int paginaId);
int pegarId();

//FERRAMENTAS BÁSICAS
int paginaCheia(ARVORE_B pagina);
int paginaNaoRaiz(ARVORE_B pagina);
int buscarPosicaoNoFilhosIds(ARVORE_B arvore, int elemento);
int paginaVazia(ARVORE_B);
void atualizarFilhosIds(ARVORE_B pagina);
void imprimirArvore(ARVORE_B pagina, int nivelAtual);
int elementoNaPagina(ARVORE_B pagina, int elemento);
void removerElemento(ARVORE_B pagina, int elemento);
ARVORE_B inserirElemento(ARVORE_B pagina, int elemento);
ARVORE_B inserirFilhoId(ARVORE_B pagina, ARVORE_B paginaFilha);
ARVORE_B buscarFolhaParaElemento(ARVORE_B pagina, int elemento);
ARVORE_B alocarNovaPagina(int tipoDePagina,int id,int paiId);
ARVORE_B gerarRaiz(int tipoDePagina);




//InserçaoFilha e Principais funçoes de inserçao
int vizinhoDireitoComVaga(ARVORE_B paginaPai, int posicaoPaginaLotada);
int vizinhoEsquerdoComVaga(ARVORE_B paginaPai, int posicaoPaginaLotada);
void jogarElementoAVizinhoEsquerdo(ARVORE_B paginaPai, ARVORE_B paginaLotada, int posicaoPaginaLotada);
void jogarElementoAVizinhoDireito(ARVORE_B paginaPai, ARVORE_B paginaLotada, int posicaoPaginaLotada);
ARVORE_B divisaoDePagina(ARVORE_B * paginaADividir);
void acrescetarNivelArvore(ARVORE_B paginaLotada);
void rebalancear(ARVORE_B paginaLotada);
ARVORE_B inserir(ARVORE_B arvore, int elemento);





//Manipuladores de Arquivo
ARVORE_B buscarPaginaArquivo(int paginaId){
	FILE * arquivo = fopen(DIRETORIO, "rb");
	ARVORE_B pagina = malloc(sizeof(PAGINA));
	if(arquivo != NULL){
		fseek(arquivo, sizeof(PAGINA)*(paginaId),SEEK_SET);
		fread(pagina, sizeof(PAGINA),1, arquivo);

		fclose(arquivo); 
		return pagina;
	}
	return NULL;
}

void atualizarPaginaArquivo(ARVORE_B pagina){
	FILE * arquivo = fopen(DIRETORIO, "r+b");
	if(arquivo == NULL){
		arquivo = fopen(DIRETORIO,"w+b");
	}

	fseek(arquivo, sizeof(PAGINA)*(pagina->id), SEEK_SET);
	fwrite(pagina, sizeof(PAGINA),1, arquivo);

	fclose(arquivo);
}

int pegarId(){
	FILE * arquivo = fopen(DIRETORIO,"rb");
	if(arquivo != NULL){
		fseek(arquivo, -sizeof(PAGINA),SEEK_END);
		ARVORE_B pagina = (ARVORE_B) malloc(sizeof(PAGINA));
		fread(pagina, sizeof(PAGINA), 1, arquivo);		
		fclose(arquivo);
		free(pagina);
		return pagina->id+1;
	}
	return 2;
}


//Ferramentas Árvore B Estrela

int paginaVazia(ARVORE_B pagina){
	return pagina == NULL;
}

int paginaNaoRaiz(ARVORE_B pagina){
	return (pagina->id > 1);
}

int paginaCheia(ARVORE_B pagina){
	return pagina->quantidadeElementos == M;
}

ARVORE_B gerarRaiz(int tipoDePagina){
	int raizIdPadrao = 1, raizPaiIdPadrao = 0;
	ARVORE_B raiz = alocarNovaPagina(tipoDePagina, raizIdPadrao, raizPaiIdPadrao);
	return raiz;
}

int buscarPosicaoNoFilhosIds(ARVORE_B pagina, int elemento){
	int i;
	for(i = 0; i < pagina->quantidadeElementos && pagina->elementos[i] <= elemento; i++);
	return i;
}

ARVORE_B buscarFolhaParaElemento(ARVORE_B pagina, int elemento){
	if(pagina->paginaInterna){
		int posicaoElemento  = buscarPosicaoNoFilhosIds(pagina, elemento);
		ARVORE_B paginaFilha = buscarPaginaArquivo(pagina->filhosIds[posicaoElemento]);
		pagina = buscarFolhaParaElemento(paginaFilha, elemento);
	}
	return pagina;
}
int elementoNaPagina(ARVORE_B pagina, int elemento){
	for(int i = 0; i < pagina->quantidadeElementos; i++)
		if(pagina->elementos[i] == elemento)
			return 1;
	return 0;

}
ARVORE_B inserirElemento(ARVORE_B pagina, int elemento){
	if(elementoNaPagina(pagina, elemento))
		return pagina;
	int posicaoElemento = buscarPosicaoNoFilhosIds(pagina, elemento);
	pagina->quantidadeElementos++;

	for(int i =  pagina->quantidadeElementos-1; i > posicaoElemento; i--){
		pagina->elementos[i] = pagina->elementos[i-1];
	}
	pagina->elementos[posicaoElemento] = elemento;
	atualizarPaginaArquivo(pagina);

	return pagina;
} 
int buscarPosicaoNosElementos(ARVORE_B pagina, int elemento){
	int i;
	for(i = 0; i < pagina->quantidadeElementos && pagina->elementos[i] != elemento; i++);
	return i;
}

ARVORE_B inserirFilhoId(ARVORE_B pagina, ARVORE_B paginaFilha){
	
	int posicaoElemento = buscarPosicaoNoFilhosIds(pagina, paginaFilha->elementos[0]);
	
	for(int i = pagina->quantidadeElementos; i > posicaoElemento ; i++){
		pagina->filhosIds[i] = pagina->filhosIds[i - 1];
	}
	pagina->filhosIds[posicaoElemento] = paginaFilha->id;

	atualizarPaginaArquivo(pagina);

	return pagina;
}
void atualizarFilhosIds(ARVORE_B pagina){
	ARVORE_B filha;
	for(int i = 0; i < pagina->quantidadeElementos + 1; i++){
		filha = buscarPaginaArquivo(pagina->filhosIds[i]);
		// if(filha->id != 1)
			filha->paiId = pagina->id;
		
		atualizarPaginaArquivo(filha);
		
	}

}
void removerElemento(ARVORE_B pagina, int elemento){
	int posicaoElemento = buscarPosicaoNosElementos(pagina, elemento);

	for(int i =posicaoElemento; i <pagina->quantidadeElementos ; i++){
		pagina->elementos[i] = pagina->elementos[i+1];
	}
	pagina->quantidadeElementos--;

	atualizarPaginaArquivo(pagina); 
}



ARVORE_B alocarNovaPagina(int tipoDePagina,int id,int paiId){
	ARVORE_B novaPagina = (ARVORE_B) malloc(sizeof(PAGINA));
	if(novaPagina != NULL){
		novaPagina->paginaInterna = tipoDePagina;
		novaPagina->quantidadeElementos = 0;
		novaPagina->id = id;
		novaPagina->paiId= paiId;
	}
	return novaPagina;
}









ARVORE_B inserir(ARVORE_B pagina,int elemento){
	if(paginaVazia(pagina)){
		pagina = gerarRaiz(0);
		inserirElemento(pagina, elemento);
	}else{
		ARVORE_B paginaDoElemento = buscarFolhaParaElemento(pagina, elemento);
		paginaDoElemento = inserirElemento(paginaDoElemento, elemento);

		if(paginaCheia(paginaDoElemento)){
			rebalancear(paginaDoElemento);
		}
		free(paginaDoElemento);

	}
	free(pagina);
	return buscarPaginaArquivo(1);
}


int vizinhoDireitoComVaga(ARVORE_B paginaPai, int posicaoPaginaLotada){
	
	if(posicaoPaginaLotada < paginaPai->quantidadeElementos - 1){
		ARVORE_B direito = buscarPaginaArquivo(paginaPai->filhosIds[posicaoPaginaLotada+1]);
		return (direito->quantidadeElementos < M-1);
	}
	return 0;
}

int vizinhoEsquerdoComVaga(ARVORE_B paginaPai, int posicaoPaginaLotada){
	
	if(posicaoPaginaLotada > 0){
		ARVORE_B esquerdo = buscarPaginaArquivo( paginaPai->filhosIds[posicaoPaginaLotada-1]);
		return (esquerdo->quantidadeElementos < M-1);
	}
	return 0;
	
}
void jogarElementoAVizinhoEsquerdo(ARVORE_B paginaPai, ARVORE_B paginaLotada, int posicaoPaginaLotada){
	paginaPai->elementos[ buscarPosicaoNosElementos(paginaPai, paginaLotada->elementos[0])] = paginaLotada->elementos[1];

	ARVORE_B esquerda = buscarPaginaArquivo(paginaPai->filhosIds[posicaoPaginaLotada-1]);
	inserirElemento(esquerda, paginaLotada->elementos[0]);
	removerElemento(paginaLotada, paginaLotada->elementos[0]);

	atualizarPaginaArquivo(paginaPai);
}

void jogarElementoAVizinhoDireito(ARVORE_B paginaPai, ARVORE_B paginaLotada, int posicaoPaginaLotada){
	ARVORE_B direita = buscarPaginaArquivo(paginaPai->filhosIds[posicaoPaginaLotada+1]);

	inserirElemento(direita, paginaLotada->elementos[M - 1]);
	removerElemento(paginaLotada, paginaLotada->elementos[M - 1]);			

	paginaPai->elementos[buscarPosicaoNosElementos(paginaPai, direita->elementos[1])] = direita->elementos[0];
	atualizarPaginaArquivo(paginaPai);

}

ARVORE_B divisaoDePagina(ARVORE_B * paginaADividir){
	int posicaoDivisaoPagina, i;
	posicaoDivisaoPagina =(int) (*paginaADividir)->quantidadeElementos/2  ;
	

	ARVORE_B novaPagina = alocarNovaPagina((*paginaADividir)->paginaInterna, ID_DISPONIVEL++ , (*paginaADividir)->paiId);

	novaPagina->quantidadeElementos = (*paginaADividir)->quantidadeElementos - posicaoDivisaoPagina;
	for(i = 0; i < novaPagina->quantidadeElementos; i++){
		novaPagina->elementos[i] = (*paginaADividir)->elementos[i + posicaoDivisaoPagina];
	}
	(*paginaADividir)->quantidadeElementos = posicaoDivisaoPagina;
	if((*paginaADividir)->paginaInterna){
		for(i = 0; i <= novaPagina->quantidadeElementos; i++)
			novaPagina->filhosIds[i] = (*paginaADividir)->filhosIds[i + posicaoDivisaoPagina];
		(*paginaADividir)->quantidadeElementos--;
	}



	atualizarPaginaArquivo(novaPagina);
	atualizarFilhosIds(novaPagina);
	atualizarPaginaArquivo((*paginaADividir));
	return novaPagina;

}

void acrescentarNivelArvore(ARVORE_B paginaLotada){
	
	ARVORE_B paginaPai;
	if(paginaNaoRaiz(paginaLotada)){
		paginaPai = buscarPaginaArquivo(paginaLotada->paiId);
	}else{
		paginaPai = gerarRaiz(1); 
		paginaLotada->paiId = 1;
		paginaLotada->id = ID_DISPONIVEL;
		ID_DISPONIVEL++; 
		inserirFilhoId(paginaPai, paginaLotada);
		atualizarFilhosIds(paginaLotada);

	}
	ARVORE_B novaPagina = divisaoDePagina(&paginaLotada);

	paginaPai = inserirElemento(paginaPai, novaPagina->elementos[0]);
	paginaPai = inserirFilhoId(paginaPai, novaPagina);
	
	if(paginaCheia(paginaPai))
		acrescentarNivelArvore(paginaPai);
		
}


void rebalancear(ARVORE_B paginaLotada){
	if(!paginaLotada->paginaInterna){

		ARVORE_B paginaPai = buscarPaginaArquivo(paginaLotada->paiId);
		int posicaoPaginaLotada = buscarPosicaoNoFilhosIds(paginaPai, paginaLotada->elementos[0]);

		if(vizinhoEsquerdoComVaga(paginaPai, posicaoPaginaLotada)){
			jogarElementoAVizinhoEsquerdo(paginaPai, paginaLotada, posicaoPaginaLotada);		
		}
		else if(vizinhoDireitoComVaga(paginaPai, posicaoPaginaLotada)){
			jogarElementoAVizinhoDireito(paginaPai, paginaLotada, posicaoPaginaLotada);
		}else{
			acrescentarNivelArvore(paginaLotada);
		}
	}
	else{
		acrescentarNivelArvore(paginaLotada);
	}	

}










void imprimirArvore(ARVORE_B pagina, int nivelAtual){
	printf("<NIVEL : %d  ID : %d PAI ID :%d INFO: [",nivelAtual,pagina->id,pagina->paiId);
	for(int i = 0 ; i < pagina->quantidadeElementos-1; i++)
		printf(" %d,",pagina->elementos[i]);
	printf("%d ]\n",pagina->elementos[pagina->quantidadeElementos-1]);

	if(pagina->paginaInterna){
		ARVORE_B paginaFilha;
		for(int i = 0; i < pagina->quantidadeElementos+1; i++){
			paginaFilha = buscarPaginaArquivo(pagina->filhosIds[i]);
			imprimirArvore(paginaFilha, nivelAtual+1);
		}
	}

}
int main(){
	ARVORE_B arvore = buscarPaginaArquivo(1);
	int entrada = 1;

	ID_DISPONIVEL = pegarId();
	printf("%d\n",ID_DISPONIVEL);



	scanf("%d",&entrada);
	do{
		arvore = inserir(arvore,entrada);
		imprimirArvore(arvore,1);
		scanf("%d",&entrada);
	}
	while(entrada != 0);

	
	return 0;
}