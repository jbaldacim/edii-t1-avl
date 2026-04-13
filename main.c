#include <stdio.h>
#include <stdlib.h>

#define max(a, b) \
	({ __typeof__ (a) _a = (a); \
		__typeof__ (b) _b = (b); \
		_a > _b ? _a : _b; })

// Estrutura de aluno
typedef struct stAluno
{
	int ra;
	// TODO - Remover esses campos, alterar nome do tipo e declarações dependentes (como nomes de funções)
	char nome[50];
	int idade;
	float notas[4];
} tAluno;

// Estrutura de nó
typedef struct stNo
{
	tAluno *aluno;
	struct stNo *esquerda;
	struct stNo *direita;
	int fb;
} tNo;

// Criar nó
tNo *criarNo(tAluno *novoAluno)
{
	tNo *novoNo = (tNo *)malloc(sizeof(tNo));

	if (!novoNo)
		exit(0);

	novoNo->esquerda = NULL;
	novoNo->direita = NULL;
	novoNo->aluno = novoAluno;

	return novoNo;
}

// Criar sub-árvore
tNo *subArvore(tNo *raiz, tAluno *novoAluno)
{
	if (!raiz)
	{
		printf("\nAluno inserido com sucesso.\n");
		return criarNo(novoAluno);
	}

	if (novoAluno->ra < raiz->aluno->ra)
		raiz->esquerda = subArvore(raiz->esquerda, novoAluno);

	else if (novoAluno->ra > raiz->aluno->ra)
		raiz->direita = subArvore(raiz->direita, novoAluno);

	else
	{
		printf("\nO RA inserido já existe no sistema.\n");
		free(novoAluno);
	}

	return raiz;
}

// Rotacionar nó para a direita
tNo *rotacaoDireita(tNo *p)
{
	tNo *q = p->esquerda;
	p->esquerda = q->direita;

	q->direita = p;
	return q;
}

// Rotacionar nó para a esquerda
tNo *rotacaoEsquerda(tNo *p)
{
	tNo *q = p->direita;
	p->direita = q->esquerda;

	q->esquerda = p;
	return q;
}

// Balancear árvore
tNo *balancear(tNo *p)
{
	if (p->fb <= -2)
	{
		if (p->esquerda->fb > 0)
			p->esquerda = rotacaoEsquerda(p->esquerda);
		p = rotacaoDireita(p);
	}
	else
	{
		if (p->direita->fb < 0)
			p->direita = rotacaoDireita(p->direita);
		p = rotacaoEsquerda(p);
	}

	return p;
}

// Atualizar fator de balanceamento
int atualizarFatBal(tNo **no)
{
	int he, hd;
	if (*no == NULL)
		return 0;

	he = atualizarFatBal(&(*no)->esquerda);
	hd = atualizarFatBal(&(*no)->direita);

	(*no)->fb = hd - he;

	if (abs((*no)->fb) >= 2)
	{
		*no = balancear(*no);
		he = atualizarFatBal(&(*no)->esquerda);
		hd = atualizarFatBal(&(*no)->direita);

		(*no)->fb = hd - he;
	}

	return max(hd, he) + 1;
}

// Função para inserir aluno
tNo *inserirAluno(tNo *raiz)
{
	int i;
	tAluno *novoAluno = (tAluno *)malloc(sizeof(tAluno));

	if (!novoAluno)
		return raiz;

	printf("Insira o RA: ");
	scanf("%d", &novoAluno->ra);

	return subArvore(raiz, novoAluno);
}

// Função para buscar aluno
void buscarAluno(tNo *raiz, int ra)
{
	if (!raiz)
	{
		printf("\nAluno não encontrado.\n");
		return;
	}

	if (ra == raiz->aluno->ra)
	{
		tAluno *aluno = raiz->aluno;
		printf("Aluno encontrado.\n");
		printf("Nome: %s\n", aluno->nome);
		printf("RA: %d\n", aluno->ra);
		return;
	}

	if (ra < raiz->aluno->ra)
		return buscarAluno(raiz->esquerda, ra);

	return buscarAluno(raiz->direita, ra);
}

// Função para exibir toda a árvore
void exibirArvore(tNo *raiz, int indentacao)
{
	if (!raiz)
		return;
	int i;

	exibirArvore(raiz->direita, indentacao + 1);
	for (i = 0; i < indentacao; i++)
		printf("   ");
	printf("%d (%d)\n", raiz->aluno->ra, raiz->fb);
	exibirArvore(raiz->esquerda, indentacao + 1);
}

// Função para percurso pré-ordem
void preOrdem(tNo *raiz, FILE **arquivo)
{
	if (!raiz)
		return;

	if (raiz->esquerda)
		fprintf(*arquivo, "%d->%d;", raiz->aluno->ra, raiz->esquerda->aluno->ra);

	else if (raiz->direita)
	{
		fprintf(*arquivo, "null [style=invis];");
		fprintf(*arquivo, "%d-> null [style=invis];", raiz->aluno->ra);
	}

	if (raiz->direita)
		fprintf(*arquivo, "%d->%d;", raiz->aluno->ra, raiz->direita->aluno->ra);

	else if (raiz->esquerda)
	{
		fprintf(*arquivo, "null [style=invis];");
		fprintf(*arquivo, "%d-> null [style=invis];", raiz->aluno->ra);
	}

	// fprintf(*arquivo, "%d", raiz->aluno->ra);
	preOrdem(raiz->esquerda, arquivo);
	preOrdem(raiz->direita, arquivo);
}

// Função para exportar árvore
void exportarArvore(tNo *raiz)
{
	FILE *arquivo;
	arquivo = fopen("AVL.dot", "w");

	if (!arquivo)
	{
		printf("Erro: não foi possível criar o arquivo.");
		return;
	}

	fprintf(arquivo, "digraph AVL {");
	fprintf(arquivo, "node [shape=circle];");
	preOrdem(raiz, &arquivo);
	fprintf(arquivo, "}");
	fclose(arquivo);

	return;
}

// Função que executa a estratégia
// maior dos menores
tAluno maiorNo(tNo *no)
{
	if (no->direita)
		return maiorNo(no->direita);
	else
		return *(no->aluno);
}

// Função que remove aluno utilizando
// método do maior dos menores
tNo *excluirAluno(tNo *no, int ra)
{
	tNo *filho = NULL;
	if (!no)
	{
		printf("\nAluno não encontrado.\n");
		return NULL;
	}

	if (ra < no->aluno->ra)
		no->esquerda = excluirAluno(no->esquerda, ra);
	else if (ra > no->aluno->ra)
		no->direita = excluirAluno(no->direita, ra);
	else
	{
		printf("\nAluno excluído com sucesso.\n");
		if (!no->direita)
		{
			filho = no->esquerda;
			free(no->aluno);
			free(no);
			return filho;
		}
		else if (!no->esquerda)
		{
			filho = no->direita;
			free(no->aluno);
			free(no);
			return filho;
		}
		else
		{
			tAluno temp = maiorNo(no->esquerda);
			*(no->aluno) = temp;
			no->esquerda = excluirAluno(no->esquerda, temp.ra);
			return no;
		}
	}
	return no;
}

// Função para apagar árvore
void apagarArvore(tNo *no)
{
	if (!no)
		return;

	apagarArvore(no->esquerda);
	apagarArvore(no->direita);

	free(no->aluno);
	free(no);
}

int main()
{
	int opcao;
	int ra;
	tNo *raiz = NULL;

	do
	{
		printf("\n===== MENU =====\n");
		printf("1 - Inserir nó\n");
		printf("2 - Buscar nó\n");
		printf("3 - Exibir a árvore\n");
		printf("4 - Exportar árvore\n");
		printf("5 - Excluir nó\n");
		printf("6 - Apagar árvore inteira\n");
		printf("7 - Sair\n");
		printf("Escolha uma opcao: ");
		scanf("%d", &opcao);

		switch (opcao)
		{
		case 1:
			raiz = inserirAluno(raiz);
			atualizarFatBal(&raiz);
			break;

		case 2:
			printf("Digite o RA para buscar: ");
			scanf("%d", &ra);
			buscarAluno(raiz, ra);
			break;

		case 3:
			printf("\n--- Estrutura da Árvore ---\n");
			exibirArvore(raiz, 0);
			printf("---------------------------\n");
			break;

		case 4:
			exportarArvore(raiz);
			printf("\nÁrvore exportada com sucesso para o arquivo AVL.dot.\n");
			break;

		case 5:
			printf("Digite o RA para excluir: ");
			scanf("%d", &ra);
			raiz = excluirAluno(raiz, ra);
			atualizarFatBal(&raiz);
			break;

		case 6:
			apagarArvore(raiz);
			printf("\nÁrvore apagada.\n");
			raiz = NULL;
			break;

		case 7:
			printf("\nEncerrando o programa.\n");
			break;

		default:
			printf("\nOpção inválida!\n");
		}

	} while (opcao != 7);

	return 0;
}