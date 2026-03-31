#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEXEMA 256

typedef enum {
    TOKEN_EOF,
    TOKEN_IDENTIFICADOR,
    TOKEN_PALAVRA_CHAVE,
    TOKEN_NUMERO_INTEIRO,
    TOKEN_NUMERO_REAL,
    TOKEN_STRING,

    TOKEN_MAIS,
    TOKEN_MENOS,
    TOKEN_MULTIPLICACAO,
    TOKEN_DIVISAO,
    TOKEN_ATRIBUICAO,
    TOKEN_IGUAL,
    TOKEN_DIFERENTE,
    TOKEN_MENOR,
    TOKEN_MENOR_IGUAL,
    TOKEN_MAIOR,
    TOKEN_MAIOR_IGUAL,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,

    TOKEN_PONTO_VIRGULA,
    TOKEN_VIRGULA,
    TOKEN_ABRE_PAREN,
    TOKEN_FECHA_PAREN,
    TOKEN_ABRE_CHAVE,
    TOKEN_FECHA_CHAVE,
    TOKEN_ABRE_COLCH,
    TOKEN_FECHA_COLCH,

    TOKEN_PONTO,
    TOKEN_HASH,
    TOKEN_E_COMERCIAL,
    TOKEN_SETA,
    TOKEN_INCREMENTO,
    TOKEN_DECREMENTO,

    TOKEN_ERRO
} TokenType;

typedef struct {
    TokenType tipo;
    char lexema[MAX_LEXEMA];
    int linha;
    int coluna;
} Token;

typedef struct {
    const char *fonte;

    int pivo;
    int batedor;

    int linha;
    int coluna;

    int linha_pivo;
    int coluna_pivo;

    int erro_comentario;
    int linha_erro;
    int coluna_erro;
} Lexer;

const char *palavras_chave[] = {
    "if", "else", "while", "for", "int", "float", "char",
    "return", "void", "typedef", "struct"
};

int quantidade_palavras_chave =
    sizeof(palavras_chave) / sizeof(palavras_chave[0]);

const char *nome_token(TokenType tipo) {
    switch (tipo) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_IDENTIFICADOR: return "IDENTIFICADOR";
        case TOKEN_PALAVRA_CHAVE: return "PALAVRA_CHAVE";
        case TOKEN_NUMERO_INTEIRO: return "NUMERO_INTEIRO";
        case TOKEN_NUMERO_REAL: return "NUMERO_REAL";
        case TOKEN_STRING: return "STRING";

        case TOKEN_MAIS: return "MAIS";
        case TOKEN_MENOS: return "MENOS";
        case TOKEN_MULTIPLICACAO: return "MULTIPLICACAO";
        case TOKEN_DIVISAO: return "DIVISAO";
        case TOKEN_ATRIBUICAO: return "ATRIBUICAO";
        case TOKEN_IGUAL: return "IGUAL";
        case TOKEN_DIFERENTE: return "DIFERENTE";
        case TOKEN_MENOR: return "MENOR";
        case TOKEN_MENOR_IGUAL: return "MENOR_IGUAL";
        case TOKEN_MAIOR: return "MAIOR";
        case TOKEN_MAIOR_IGUAL: return "MAIOR_IGUAL";
        case TOKEN_AND: return "AND";
        case TOKEN_OR: return "OR";
        case TOKEN_NOT: return "NOT";

        case TOKEN_PONTO_VIRGULA: return "PONTO_VIRGULA";
        case TOKEN_VIRGULA: return "VIRGULA";
        case TOKEN_ABRE_PAREN: return "ABRE_PAREN";
        case TOKEN_FECHA_PAREN: return "FECHA_PAREN";
        case TOKEN_ABRE_CHAVE: return "ABRE_CHAVE";
        case TOKEN_FECHA_CHAVE: return "FECHA_CHAVE";
        case TOKEN_ABRE_COLCH: return "ABRE_COLCH";
        case TOKEN_FECHA_COLCH: return "FECHA_COLCH";

        case TOKEN_PONTO: return "PONTO";
        case TOKEN_HASH: return "HASH";
        case TOKEN_E_COMERCIAL: return "E_COMERCIAL";
        case TOKEN_SETA: return "SETA";
        case TOKEN_INCREMENTO: return "INCREMENTO";
        case TOKEN_DECREMENTO: return "DECREMENTO";

        case TOKEN_ERRO: return "ERRO";
        default: return "DESCONHECIDO";
    }
}

void inicializar_lexer(Lexer *lexer, const char *fonte) {
    lexer->fonte = fonte;
    lexer->pivo = 0;
    lexer->batedor = 0;
    lexer->linha = 1;
    lexer->coluna = 1;
    lexer->linha_pivo = 1;
    lexer->coluna_pivo = 1;
    lexer->erro_comentario = 0;
    lexer->linha_erro = 0;
    lexer->coluna_erro = 0;
}

char atual(Lexer *lexer) {
    return lexer->fonte[lexer->batedor];
}

char proximo(Lexer *lexer) {
    if (lexer->fonte[lexer->batedor] == '\0') {
        return '\0';
    }
    return lexer->fonte[lexer->batedor + 1];
}

char avancar(Lexer *lexer) {
    char c = lexer->fonte[lexer->batedor];

    if (c == '\n') {
        lexer->linha++;
        lexer->coluna = 1;
    } else if (c != '\0') {
        lexer->coluna++;
    }

    lexer->batedor++;
    return c;
}

void marcar_inicio_lexema(Lexer *lexer) {
    lexer->pivo = lexer->batedor;
    lexer->linha_pivo = lexer->linha;
    lexer->coluna_pivo = lexer->coluna;
}

int eh_palavra_chave(const char *texto) {
    for (int i = 0; i < quantidade_palavras_chave; i++) {
        if (strcmp(texto, palavras_chave[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

Token criar_token_mensagem(TokenType tipo, const char *mensagem, int linha, int coluna) {
    Token token;
    token.tipo = tipo;
    strncpy(token.lexema, mensagem, MAX_LEXEMA - 1);
    token.lexema[MAX_LEXEMA - 1] = '\0';
    token.linha = linha;
    token.coluna = coluna;
    return token;
}

Token criar_token_intervalo(Lexer *lexer, TokenType tipo) {
    Token token;
    int tamanho = lexer->batedor - lexer->pivo;

    if (tamanho < 0) tamanho = 0;
    if (tamanho >= MAX_LEXEMA) tamanho = MAX_LEXEMA - 1;

    token.tipo = tipo;
    strncpy(token.lexema, lexer->fonte + lexer->pivo, tamanho);
    token.lexema[tamanho] = '\0';
    token.linha = lexer->linha_pivo;
    token.coluna = lexer->coluna_pivo;

    return token;
}

void pular_espacos_e_comentarios(Lexer *lexer) {
    while (1) {
        char c = atual(lexer);

        while (isspace((unsigned char)c)) {
            avancar(lexer);
            c = atual(lexer);
        }

        if (c == '/' && proximo(lexer) == '/') {
            while (atual(lexer) != '\n' && atual(lexer) != '\0') {
                avancar(lexer);
            }
            continue;
        }

        if (c == '/' && proximo(lexer) == '*') {
            int linha_inicio = lexer->linha;
            int coluna_inicio = lexer->coluna;

            avancar(lexer); // /
            avancar(lexer); // *

            while (!(atual(lexer) == '*' && proximo(lexer) == '/') &&
                   atual(lexer) != '\0') {
                avancar(lexer);
            }

            if (atual(lexer) == '\0') {
                lexer->erro_comentario = 1;
                lexer->linha_erro = linha_inicio;
                lexer->coluna_erro = coluna_inicio;
                return;
            }

            avancar(lexer); // *
            avancar(lexer); // /
            continue;
        }

        break;
    }
}

Token ler_identificador_ou_palavra_chave(Lexer *lexer) {
    marcar_inicio_lexema(lexer);

    while (isalnum((unsigned char)atual(lexer)) || atual(lexer) == '_') {
        avancar(lexer);
    }

    Token token = criar_token_intervalo(lexer, TOKEN_IDENTIFICADOR);

    if (eh_palavra_chave(token.lexema)) {
        token.tipo = TOKEN_PALAVRA_CHAVE;
    }

    return token;
}

Token ler_numero(Lexer *lexer) {
    int tem_ponto = 0;
    marcar_inicio_lexema(lexer);

    while (isdigit((unsigned char)atual(lexer)) ||
           (atual(lexer) == '.' && !tem_ponto)) {
        if (atual(lexer) == '.') {
            tem_ponto = 1;
        }
        avancar(lexer);
    }

    if (tem_ponto) {
        return criar_token_intervalo(lexer, TOKEN_NUMERO_REAL);
    }

    return criar_token_intervalo(lexer, TOKEN_NUMERO_INTEIRO);
}

Token ler_string(Lexer *lexer) {
    int inicio_linha, inicio_coluna;
    marcar_inicio_lexema(lexer);

    inicio_linha = lexer->linha;
    inicio_coluna = lexer->coluna;

    avancar(lexer); // abre "

    marcar_inicio_lexema(lexer); // lexema começa depois da aspas

    while (atual(lexer) != '"' && atual(lexer) != '\0') {
        if (atual(lexer) == '\\' && proximo(lexer) != '\0') {
            avancar(lexer);
            avancar(lexer);
        } else {
            avancar(lexer);
        }
    }

    if (atual(lexer) == '\0') {
        return criar_token_mensagem(
            TOKEN_ERRO,
            "String nao terminada",
            inicio_linha,
            inicio_coluna
        );
    }

    Token token = criar_token_intervalo(lexer, TOKEN_STRING);
    avancar(lexer); // fecha "

    return token;
}

Token proximo_token(Lexer *lexer) {
    pular_espacos_e_comentarios(lexer);

    if (lexer->erro_comentario) {
        lexer->erro_comentario = 0;
        return criar_token_mensagem(
            TOKEN_ERRO,
            "Comentario de bloco nao fechado",
            lexer->linha_erro,
            lexer->coluna_erro
        );
    }

    if (atual(lexer) == '\0') {
        return criar_token_mensagem(TOKEN_EOF, "EOF", lexer->linha, lexer->coluna);
    }

    if (isalpha((unsigned char)atual(lexer)) || atual(lexer) == '_') {
        return ler_identificador_ou_palavra_chave(lexer);
    }

    if (isdigit((unsigned char)atual(lexer))) {
        return ler_numero(lexer);
    }

    if (atual(lexer) == '"') {
        return ler_string(lexer);
    }

    marcar_inicio_lexema(lexer);

    switch (atual(lexer)) {
        case '+':
            avancar(lexer);
            if (atual(lexer) == '+') {
                avancar(lexer);
                return criar_token_intervalo(lexer, TOKEN_INCREMENTO);
            }
            return criar_token_intervalo(lexer, TOKEN_MAIS);

        case '-':
            avancar(lexer);
            if (atual(lexer) == '>') {
                avancar(lexer);
                return criar_token_intervalo(lexer, TOKEN_SETA);
            }
            if (atual(lexer) == '-') {
                avancar(lexer);
                return criar_token_intervalo(lexer, TOKEN_DECREMENTO);
            }
            return criar_token_intervalo(lexer, TOKEN_MENOS);

        case '*':
            avancar(lexer);
            return criar_token_intervalo(lexer, TOKEN_MULTIPLICACAO);

        case '/':
            avancar(lexer);
            return criar_token_intervalo(lexer, TOKEN_DIVISAO);

        case '=':
            avancar(lexer);
            if (atual(lexer) == '=') {
                avancar(lexer);
                return criar_token_intervalo(lexer, TOKEN_IGUAL);
            }
            return criar_token_intervalo(lexer, TOKEN_ATRIBUICAO);

        case '!':
            avancar(lexer);
            if (atual(lexer) == '=') {
                avancar(lexer);
                return criar_token_intervalo(lexer, TOKEN_DIFERENTE);
            }
            return criar_token_intervalo(lexer, TOKEN_NOT);

        case '<':
            avancar(lexer);
            if (atual(lexer) == '=') {
                avancar(lexer);
                return criar_token_intervalo(lexer, TOKEN_MENOR_IGUAL);
            }
            return criar_token_intervalo(lexer, TOKEN_MENOR);

        case '>':
            avancar(lexer);
            if (atual(lexer) == '=') {
                avancar(lexer);
                return criar_token_intervalo(lexer, TOKEN_MAIOR_IGUAL);
            }
            return criar_token_intervalo(lexer, TOKEN_MAIOR);

        case '&':
            avancar(lexer);
            if (atual(lexer) == '&') {
                avancar(lexer);
                return criar_token_intervalo(lexer, TOKEN_AND);
            }
            return criar_token_intervalo(lexer, TOKEN_E_COMERCIAL);

        case '|':
            avancar(lexer);
            if (atual(lexer) == '|') {
                avancar(lexer);
                return criar_token_intervalo(lexer, TOKEN_OR);
            }
            return criar_token_mensagem(
                TOKEN_ERRO,
                "| invalido",
                lexer->linha_pivo,
                lexer->coluna_pivo
            );

        case ';':
            avancar(lexer);
            return criar_token_intervalo(lexer, TOKEN_PONTO_VIRGULA);

        case ',':
            avancar(lexer);
            return criar_token_intervalo(lexer, TOKEN_VIRGULA);

        case '(':
            avancar(lexer);
            return criar_token_intervalo(lexer, TOKEN_ABRE_PAREN);

        case ')':
            avancar(lexer);
            return criar_token_intervalo(lexer, TOKEN_FECHA_PAREN);

        case '{':
            avancar(lexer);
            return criar_token_intervalo(lexer, TOKEN_ABRE_CHAVE);

        case '}':
            avancar(lexer);
            return criar_token_intervalo(lexer, TOKEN_FECHA_CHAVE);

        case '[':
            avancar(lexer);
            return criar_token_intervalo(lexer, TOKEN_ABRE_COLCH);

        case ']':
            avancar(lexer);
            return criar_token_intervalo(lexer, TOKEN_FECHA_COLCH);

        case '.':
            avancar(lexer);
            return criar_token_intervalo(lexer, TOKEN_PONTO);

        case '#':
            avancar(lexer);
            return criar_token_intervalo(lexer, TOKEN_HASH);

        default: {
            char erro[64];
            char c = atual(lexer);
            snprintf(erro, sizeof(erro), "Simbolo invalido: %c", c);
            avancar(lexer);
            return criar_token_mensagem(
                TOKEN_ERRO,
                erro,
                lexer->linha_pivo,
                lexer->coluna_pivo
            );
        }
    }
}

char *ler_arquivo(const char *nome_arquivo) {
    FILE *f = fopen(nome_arquivo, "rb");
    if (!f) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    rewind(f);

    char *buffer = (char *)malloc(tamanho + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, tamanho, f);
    buffer[tamanho] = '\0';
    fclose(f);

    return buffer;
}

int main(int argc, char *argv[]) {
    char *codigo;
    Lexer lexer;
    Token token;

    if (argc < 2) {
        printf("Uso: %s arquivo.txt\n", argv[0]);
        return 1;
    }

    codigo = ler_arquivo(argv[1]);
    if (!codigo) {
        printf("Erro ao abrir o arquivo: %s\n", argv[1]);
        return 1;
    }

    inicializar_lexer(&lexer, codigo);

    do {
        token = proximo_token(&lexer);
        printf("Linha %d, Coluna %d -> %-18s : %s\n",
               token.linha, token.coluna, nome_token(token.tipo), token.lexema);
    } while (token.tipo != TOKEN_EOF);

    free(codigo);
    return 0;
}