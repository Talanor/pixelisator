CC	=	gcc
RM	=	rm -f
CFLAGS	=	-W -Wall -Wextra -pedantic
LDFLAGS	=	-lSDL2 -lSDL2_image
SRC	=	$(wildcard *.c)
OBJ	=	$(SRC:.c=.o)
NAME	=	pixelisator

all:	$(NAME)

$(NAME):	$(OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

%.o:	%.c
	$(CC) -c $^ $(CFLAGS) -o $@

clean:
	$(RM) $(OBJ)

fclean:	clean
	$(RM) $(NAME)

re:	fclean all
