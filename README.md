# minishell-cso


### readline dep
sudo apt-get install libreadline-dev

## Lessico

spaziatori: spazi e tab (ad eccezioni di quelli nelle quotes)

quotes: '...' e "..." -> il contenuto viene copiato letteralmente

backlash: \char -> il char viene copiato letteralmente (\\ -> viene copiato \)

Tipi di Token:
- WORD

- T_PIPE = |

- T_RED_OUT = >
- T_RED_OUT_APP = >>
- T_RED_IN = <
- T_HEREDOC = <<

Errori lessicali:
- Quotes non chiuse
- Nessun carattere dopo \
- Input eccede la lunghezza massima

