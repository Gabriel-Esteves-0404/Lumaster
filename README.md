# Lumaster - Sistema de Gerenciamento de Viagens Escolares

O **Lumaster** é um sistema completo desenvolvido em **C com GTK 3**, voltado para o gerenciamento eficaz de viagens escolares, envolvendo controle de usuários, alunos, turmas, pagamentos e exportações para planilhas.

## 🧠 Principais Funcionalidades

### 🔑 Gestão de Usuários

* Autenticação com níveis de permissão (Administrador e Convidado)
* Cadastro, edição e exclusão de usuários
* Exportação dos usuários para CSV (`usuarios.csv`)

### 🎒 Gestão de Alunos

* Cadastro completo dos alunos
* Associação com viagens e turmas específicas
* Controle detalhado de pagamentos e parcelas (status: quitado, pendente, em dia)
* Filtro inteligente por CPF

### 🚍 Gestão de Viagens

* Cadastro completo de viagens (escola, destino, data, duração)
* Gestão detalhada de custos (transporte, hospedagem, alimentação, extras)
* Cálculo automático e transparente dos custos por aluno
* Exportação automática de planilhas de embarque, seguro e custos para arquivos CSV

### 💳 Gestão Financeira

* Controle visual detalhado de pagamentos
* Status claro de parcelas pagas e pendentes
* Interface dinâmica para atualização instantânea

## 🗃️ Estrutura do Repositório

```plaintext
lumaster/
├── src/          # Código-fonte (.c, .h)
├── .gitignore    # Arquivos ignorados pelo Git
├── LICENSE       # Licença MIT
└── README.md     # Documentação principal do projeto
```

### 📂 Explicação dos Arquivos em `src/`

| Arquivo                   | Responsabilidade                                                                 |
|---------------------------|----------------------------------------------------------------------------------|
| `main_gtk.c`              | Tela inicial, login, cadastro e navegação principal (GTK)                        |
| `lumaster.c`              | Inicialização e controle principal pós-login                                     |
| `usuarios_gui.c/.h`       | Gerenciamento de usuários (cadastro, login, edição)                              |
| `funcoes_alunos.c`        | Manipulação de dados internos dos alunos                                         |
| `funcoes_viagens.c`       | Manipulação e cálculos relacionados às viagens                                   |
| `gui_*.c/.h`              | Interfaces gráficas específicas para cada módulo (alunos, pagamentos, viagens…)  |
| `utils.c/.h`              | Funções auxiliares gerais (diálogos, validações etc.)                            |
| `alunos_gerenciador.h`    | Definições e protótipos do módulo alunos                                         |
| `gerenciador_viagens.h`   | Definições e protótipos do módulo viagens                                        |
| `viagensmain.c/.h`        | Controle da navegação e integração das páginas internas                          |



## 🚀 Como Rodar o Sistema

### Pré-requisitos

* Compilador GCC
* GTK 3 (pacote de desenvolvimento) e `pkg-config`
  * Debian/Ubuntu: `sudo apt install build-essential libgtk-3-dev pkg-config`
  * Windows: MSYS2 (`pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-gcc pkg-config`)

### Compilação

```bash
mkdir -p bin
gcc src/*.c -o bin/lumaster $(pkg-config --cflags --libs gtk+-3.0)
```

### Execução

Execute a partir de uma pasta onde o programa possa gravar arquivos, pois ele salva os dados no diretório atual:

```bash
./bin/lumaster
```

## 📂 Sobre os Dados

O repositório não inclui dados. Na primeira execução o sistema cria, no diretório de execução, os arquivos abaixo conforme você cadastra informações:

| Arquivo             | Descrição                                   |
| ------------------- | ------------------------------------------- |
| `usuarios.dat`      | Dados binários dos usuários                 |
| `alunos_data.bin`   | Dados binários dos alunos                   |
| `viagens_data.bin`  | Dados binários das viagens                  |
| `*.csv`             | Exportações (usuários, embarque, seguro, custos) |

## 🖥️ Interface Gráfica (GTK 3)

O sistema utiliza GTK 3 para oferecer uma interface intuitiva e organizada, permitindo fácil acesso e uso das funcionalidades:

* Telas bem distribuídas e organizadas
* Interface responsiva e visualmente clara
* Uso dinâmico de cores e avisos para facilitar a interação

## ⚖️ Licença

Este projeto está sob licença [MIT](LICENSE), permitindo o uso, modificação e distribuição livre do sistema.


Integrantes:
- Gabriel Esteves
- Kayque Moreira
- Anne Beatriz
- Maria Clara Sousa
- Arthur Levy
- Arthur Pessoa Lira
