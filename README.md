# Lumastur - Sistema de Gerenciamento de Viagens Escolares

O **Lumastur** é um sistema completo desenvolvido em **C com GTK 4**, voltado para o gerenciamento eficaz de viagens escolares, envolvendo controle de usuários, alunos, turmas, pagamentos e exportações para planilhas.

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
Lumastur/
├── src/          # Código-fonte (.c, .h)
├── data/         # Arquivos de dados e planilhas (.bin, .dat, .csv)
├── bin/          # Executável compilado (lumaster.exe)
├── .vscode/      # Configuração do VS Code
├── docs/         # Documentação adicional (imagens, manuais)
├── .gitignore    # Arquivo de exclusão do Git
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

### Com o executável já compilado:

Basta abrir a pasta `bin` e executar:

```plaintext
lumaster.exe
```

### Compilação usando Makefile:

Se desejar recompilar o projeto (necessário ter GCC e GTK4 instalados):

```bash
make
./bin/lumaster.exe
```

## 📂 Sobre os Dados

Todos os arquivos importantes estão na pasta `data/`:

| Arquivo                          | Descrição                    |
| -------------------------------- | ---------------------------- |
| `usuarios.dat`                   | Dados binários dos usuários  |
| `usuarios.csv`                   | Exportação de usuários       |
| `alunos_data.bin`                | Dados binários dos alunos    |
| `viagens_data.bin`               | Dados binários das viagens   |
| `Custos_Teste.csv`               | Planilha exemplo de custos   |
| `Planilha_de_Embarque-Teste.csv` | Planilha exemplo de embarque |
| `Seguro_Viagem-Teste.csv`        | Planilha exemplo de seguro   |

## 🖥️ Interface Gráfica (GTK 4)

O sistema utiliza GTK 4 para oferecer uma interface intuitiva e organizada, permitindo fácil acesso e uso das funcionalidades:

* Telas bem distribuídas e organizadas
* Interface responsiva e visualmente clara
* Uso dinâmico de cores e avisos para facilitar a interação

*(Recomenda-se adicionar prints de telas na pasta `docs/` e incluir aqui para demonstração.)*

## ⚖️ Licença

Este projeto está sob licença [MIT](LICENSE), permitindo o uso, modificação e distribuição livre do sistema.
