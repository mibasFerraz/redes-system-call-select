# redes-system-call-select

**Conteúdo da Disciplina**: Trabalho 1<br>

## Aluno
| Matrícula   |         Aluno          |
| ----------- | ---------------------- |
| 18/0105345  |  Jonathan Jorge Barbosa Oliveira     |
| 18/0103792  |  Júlia Farias Sousa    |
| 18/0103580  |  Lucas Lima Ferraz     |

## Sobre 
Trabalho relativo ao que se encontra no pdf na pasta assets.

## Instalação 
**Linguagem**: C<br>

## Clonar Repositório
Clonar o repositório e rodar paralelamente em terminais diferentes o servidor e o cliente.
```bash
git clone https://github.com/mibasFerraz/redes-system-call-select/blob/main/README.md 
```
## Para compilar o servidor 
```bash
gcc servidor.c -o servidor
```
## Para rodar o servidor no ip desejado 
```bash
./servidor 127.0.0.1 8000
```
## Para conectar o cliente
```bash
telnet 127.0.0.1 8000
```