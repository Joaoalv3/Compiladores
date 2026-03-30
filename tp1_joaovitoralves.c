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
    int pos;
    int linha;
    int coluna;
} Lexer;

const char *palavras_chave[] = {
    "if", "else", "while", "for", "int", "float", "char",
    "return", "void", "typedef", "struct"
};

int quantidade_palavras_chave = sizeof(palavras_chave) / sizeof(palavras_chave[0]);

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
    lexer->pos = 0;
    lexer->linha = 1;
    lexer->coluna = 1;
}

char atual(Lexer *lexer) {
    return lexer->fonte[lexer->pos];
}

char proximo(Lexer *lexer) {
    if (lexer->fonte[lexer->pos] == '\0') return '\0';
    return lexer->fonte[lexer->pos + 1];
}

char avancar(Lexer *lexer) {
    char c = lexer->fonte[lexer->pos];

    if (c == '\n') {
        lexer->linha++;
        lexer->coluna = 1;
    } else if (c != '\0') {
        lexer->coluna++;
    }

    lexer->pos++;
    return c;
}

int eh_palavra_chave(const char *texto) {
    for (int i = 0; i < quantidade_palavras_chave; i++) {
        if (strcmp(texto, palavras_chave[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void pular_espacos_e_comentarios(Lexer *lexer) {
    while (1) {
        char c = atual(lexer);

        while (isspace(c)) {
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
            avancar(lexer); // /
            avancar(lexer); // *

            while (!(atual(lexer) == '*' && proximo(lexer) == '/') &&
                   atual(lexer) != '\0') {
                avancar(lexer);
            }

            if (atual(lexer) == '*' && proximo(lexer) == '/') {
                avancar(lexer); // *
                avancar(lexer); // /
            }
            continue;
        }

        break;
    }
}

Token criar_token(TokenType tipo, const char *lexema, int linha, int coluna) {
    Token token;
    token.tipo = tipo;
    strncpy(token.lexema, lexema, MAX_LEXEMA - 1);
    token.lexema[MAX_LEXEMA - 1] = '\0';
    token.linha = linha;
    token.coluna = coluna;
    return token;
}

Token ler_identificador_ou_palavra_chave(Lexer *lexer) {
    char buffer[MAX_LEXEMA];
    int i = 0;
    int linha_inicio = lexer->linha;
    int coluna_inicio = lexer->coluna;

    while (isalnum(atual(lexer)) || atual(lexer) == '_') {
        if (i < MAX_LEXEMA - 1) {
            buffer[i++] = avancar(lexer);
        } else {
            avancar(lexer);
        }
    }

    buffer[i] = '\0';

    if (eh_palavra_chave(buffer)) {
        return criar_token(TOKEN_PALAVRA_CHAVE, buffer, linha_inicio, coluna_inicio);
    }

    return criar_token(TOKEN_IDENTIFICADOR, buffer, linha_inicio, coluna_inicio);
}

Token ler_numero(Lexer *lexer) {
    char buffer[MAX_LEXEMA];
    int i = 0;
    int linha_inicio = lexer->linha;
    int coluna_inicio = lexer->coluna;
    int tem_ponto = 0;

    while (isdigit(atual(lexer)) || (atual(lexer) == '.' && !tem_ponto)) {
        if (atual(lexer) == '.') {
            tem_ponto = 1;
        }

        if (i < MAX_LEXEMA - 1) {
            buffer[i++] = avancar(lexer);
        } else {
            avancar(lexer);
        }
    }

    buffer[i] = '\0';

    if (tem_ponto) {
        return criar_token(TOKEN_NUMERO_REAL, buffer, linha_inicio, coluna_inicio);
    }

    return criar_token(TOKEN_NUMERO_INTEIRO, buffer, linha_inicio, coluna_inicio);
}

Token ler_string(Lexer *lexer) {
    char buffer[MAX_LEXEMA];
    int i = 0;
    int linha_inicio = lexer->linha;
    int coluna_inicio = lexer->coluna;

    avancar(lexer); // consome "

    while (atual(lexer) != '"' && atual(lexer) != '\0') {
        if (atual(lexer) == '\\' && proximo(lexer) != '\0') {
            if (i < MAX_LEXEMA - 1) buffer[i++] = avancar(lexer);
            if (i < MAX_LEXEMA - 1) buffer[i++] = avancar(lexer);
        } else {
            if (i < MAX_LEXEMA - 1) {
                buffer[i++] = avancar(lexer);
            } else {
                avancar(lexer);
            }
        }
    }

    if (atual(lexer) == '"') {
        avancar(lexer); // consome "
        buffer[i] = '\0';
        return criar_token(TOKEN_STRING, buffer, linha_inicio, coluna_inicio);
    }

    buffer[i] = '\0';
    return criar_token(TOKEN_ERRO, "String nao terminada", linha_inicio, coluna_inicio);
}

Token proximo_token(Lexer *lexer) {
    pular_espacos_e_comentarios(lexer);

    int linha_inicio = lexer->linha;
    int coluna_inicio = lexer->coluna;
    char c = atual(lexer);

    if (c == '\0') {
        return criar_token(TOKEN_EOF, "EOF", linha_inicio, coluna_inicio);
    }

    if (isalpha(c) || c == '_') {
        return ler_identificador_ou_palavra_chave(lexer);
    }

    if (isdigit(c)) {
        return ler_numero(lexer);
    }

    if (c == '"') {
        return ler_string(lexer);
    }

    switch (c) {
        case '+':
            if (proximo(lexer) == '+') {
                avancar(lexer);
                avancar(lexer);
                return criar_token(TOKEN_INCREMENTO, "++", linha_inicio, coluna_inicio);
            }
            avancar(lexer);
            return criar_token(TOKEN_MAIS, "+", linha_inicio, coluna_inicio);

        case '-':
            if (proximo(lexer) == '>') {
                avancar(lexer);
                avancar(lexer);
                return criar_token(TOKEN_SETA, "->", linha_inicio, coluna_inicio);
            }
            if (proximo(lexer) == '-') {
                avancar(lexer);
                avancar(lexer);
                return criar_token(TOKEN_DECREMENTO, "--", linha_inicio, coluna_inicio);
            }
            avancar(lexer);
            return criar_token(TOKEN_MENOS, "-", linha_inicio, coluna_inicio);

        case '*':
            avancar(lexer);
            return criar_token(TOKEN_MULTIPLICACAO, "*", linha_inicio, coluna_inicio);

        case '/':
            avancar(lexer);
            return criar_token(TOKEN_DIVISAO, "/", linha_inicio, coluna_inicio);

        case '=':
            if (proximo(lexer) == '=') {
                avancar(lexer);
                avancar(lexer);
                return criar_token(TOKEN_IGUAL, "==", linha_inicio, coluna_inicio);
            }
            avancar(lexer);
            return criar_token(TOKEN_ATRIBUICAO, "=", linha_inicio, coluna_inicio);

        case '!':
            if (proximo(lexer) == '=') {
                avancar(lexer);
                avancar(lexer);
                return criar_token(TOKEN_DIFERENTE, "!=", linha_inicio, coluna_inicio);
            }
            avancar(lexer);
            return criar_token(TOKEN_NOT, "!", linha_inicio, coluna_inicio);

        case '<':
            if (proximo(lexer) == '=') {
                avancar(lexer);
                avancar(lexer);
                return criar_token(TOKEN_MENOR_IGUAL, "<=", linha_inicio, coluna_inicio);
            }
            avancar(lexer);
            return criar_token(TOKEN_MENOR, "<", linha_inicio, coluna_inicio);

        case '>':
            if (proximo(lexer) == '=') {
                avancar(lexer);
                avancar(lexer);
                return criar_token(TOKEN_MAIOR_IGUAL, ">=", linha_inicio, coluna_inicio);
            }
            avancar(lexer);
            return criar_token(TOKEN_MAIOR, ">", linha_inicio, coluna_inicio);

        case '&':
            if (proximo(lexer) == '&') {
                avancar(lexer);
                avancar(lexer);
                return criar_token(TOKEN_AND, "&&", linha_inicio, coluna_inicio);
            }
            avancar(lexer);
            return criar_token(TOKEN_E_COMERCIAL, "&", linha_inicio, coluna_inicio);

        case '|':
            if (proximo(lexer) == '|') {
                avancar(lexer);
                avancar(lexer);
                return criar_token(TOKEN_OR, "||", linha_inicio, coluna_inicio);
            }
            avancar(lexer);
            return criar_token(TOKEN_ERRO, "| invalido", linha_inicio, coluna_inicio);

        case ';':
            avancar(lexer);
            return criar_token(TOKEN_PONTO_VIRGULA, ";", linha_inicio, coluna_inicio);

        case ',':
            avancar(lexer);
            return criar_token(TOKEN_VIRGULA, ",", linha_inicio, coluna_inicio);

        case '(':
            avancar(lexer);
            return criar_token(TOKEN_ABRE_PAREN, "(", linha_inicio, coluna_inicio);

        case ')':
            avancar(lexer);
            return criar_token(TOKEN_FECHA_PAREN, ")", linha_inicio, coluna_inicio);

        case '{':
            avancar(lexer);
            return criar_token(TOKEN_ABRE_CHAVE, "{", linha_inicio, coluna_inicio);

        case '}':
            avancar(lexer);
            return criar_token(TOKEN_FECHA_CHAVE, "}", linha_inicio, coluna_inicio);

        case '[':
            avancar(lexer);
            return criar_token(TOKEN_ABRE_COLCH, "[", linha_inicio, coluna_inicio);

        case ']':
            avancar(lexer);
            return criar_token(TOKEN_FECHA_COLCH, "]", linha_inicio, coluna_inicio);

        case '.':
            avancar(lexer);
            return criar_token(TOKEN_PONTO, ".", linha_inicio, coluna_inicio);

        case '#':
            avancar(lexer);
            return criar_token(TOKEN_HASH, "#", linha_inicio, coluna_inicio);

        default: {
            char erro[64];
            snprintf(erro, sizeof(erro), "Simbolo invalido: %c", c);
            avancar(lexer);
            return criar_token(TOKEN_ERRO, erro, linha_inicio, coluna_inicio);
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
    char *codigo = NULL;

    if (argc < 2) {
        printf("Uso: %s arquivo.txt\n", argv[0]);
        return 1;
    }

    codigo = ler_arquivo(argv[1]);
    if (!codigo) {
        printf("Erro ao abrir o arquivo: %s\n", argv[1]);
        return 1;
    }

    Lexer lexer;
    inicializar_lexer(&lexer, codigo);

    Token token;
    do {
        token = proximo_token(&lexer);
        printf("Linha %d, Coluna %d -> %-18s : %s\n",
               token.linha, token.coluna, nome_token(token.tipo), token.lexema);
    } while (token.tipo != TOKEN_EOF);

    free(codigo);
    return 0;
}