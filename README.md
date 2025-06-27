# CangaCompiler

<div align='center'>
<h2>
Compiladores - 2025.1

Universidade SENAI Cimatec

</h2>

</div>

- Davih de Andrade Machado Borges Santos
- Felipe Azevedo Ribeiro
- Gabriel Trindade Santana
- Pedro Quadros de Freitas

## Como executar

```bash
g++ -std=c++17 ./main.cpp -I include -o CangaCompiler
```

```bash
./CangaCompiler <file_name>.251
```

## Sobre o Compilador

O **CangaCompiler** é um compilador desenvolvido para a disciplina de Compiladores da Universidade SENAI Cimatec. Este projeto implementa as fases de análise léxica e sintática de uma linguagem de programação customizada, gerando relatórios detalhados sobre os tokens encontrados e a tabela de símbolos.

### Principais Funcionalidades

#### 🔍 **Análise Léxica**

- Reconhecimento de tokens (palavras-chave, identificadores, constantes, operadores)
- Geração do arquivo `.LEX` com todos os lexemas encontrados no código fonte
- Tratamento de comentários e espaços em branco
- Validação de identificadores (não podem ser palavras reservadas)

#### 📋 **Tabela de Símbolos**

- Geração do arquivo `.TAB` com informações detalhadas sobre variáveis e funções
- Rastreamento de linhas onde cada símbolo é utilizado
- Classificação automática de tipos (inteiro, real, string, caractere, booleano)
- Suporte a arrays com especificação de tamanho

#### 🏗️ **Estruturas de Controle**

- **Declaração de Variáveis**: Suporte a tipos básicos e arrays
  ```plaintext
  varType integer: x, y, z;
  varType real[]: array1[10], array2[20];
  ```
- **Funções**: Declaração com parâmetros tipados
  ```plaintext
  FUNCTYPE integer: soma(paramType integer: a, b; paramType real: c, d)
  ```
- **Estruturas de Repetição**: Loop WHILE com validação de sintaxe
  ```plaintext
  WHILE (condicao) {
      // código
  }
  ENDWHILE
  ```

#### ✅ **Validações Sintáticas**

- Verificação de tipos em declarações de variáveis e parâmetros
- Validação de estrutura de blocos (funções, loops)
- Controle de contexto para declarações
- Tratamento de erros com mensagens claras em português

#### 📊 **Relatórios Gerados**

- **Arquivo .LEX**: Lista completa de tokens com tipo, lexema, índice na tabela e linha
- **Arquivo .TAB**: Tabela de símbolos com informações de tipo, linhas de uso e códigos de classificação

### Tecnologias Utilizadas

- **Linguagem**: C++17
- **Estruturas de Dados**: Maps, Vectors, Stacks
- **Processamento**: Análise léxica customizada, tabela de símbolos

Este compilador demonstra os conceitos fundamentais de análise léxica e sintática, sendo uma ferramenta educacional para compreensão dos processos de compilação.
