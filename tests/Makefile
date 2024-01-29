NAME		= tester
ARGS 		= 
CXX			= @c++
CXXFLAGS	= -Wall -Wextra -Werror -MMD -MP -pedantic -g
 
INCLUDES 	= ../Includes

T_SRCS		= 	clientTests.cpp \
				joinTests.cpp \
				kickTests.cpp \
				main.cpp \
				partTests.cpp \
				TestUtils.cpp

SRCS		=	Irc.cpp \
				IrcReply.cpp \
				AServer.cpp \
				Channel.cpp \
				Client.cpp \
				utils_string.cpp

SRC_DIR 	= ../Srcs
SRCS 		:= $(addprefix $(SRC_DIR)/, $(SRCS))

OBJ_DIR 	= ./objects_and_dependencies
OBJFILES 	= $(notdir $(SRCS:.cpp=.o))
OBJS 		= $(addprefix $(OBJ_DIR)/, $(OBJFILES))

T_OBJFILES 	= $(T_SRCS:.cpp=.o)
T_OBJS 		= $(addprefix $(OBJ_DIR)/, $(T_OBJFILES))

DEPS 		:= $(OBJS:.o=.d)

.PHONY: all clean fclean re run va scan fun

#Build________________________________________________________
all: $(OBJ_DIR) $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(NAME): $(OBJS) $(T_OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(T_OBJS) -o $(NAME)
	@echo "$(GREEN)Executable \"$(NAME)\" succesfully created.$(NC)"

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -I$(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -I$(INCLUDES) -c $< -o $@

#General________________________________________________________
clean:
	rm -rf $(OBJ_DIR)
	@echo "$(MAGENTA)Obj directory removed.$(NC)"

fclean: clean
	rm -f $(NAME) test_errors.log
	@echo "$(MAGENTA)Deleted executable named: $(NAME)$(NC)"

re: fclean all
	@echo "$(GREEN)Recompilation successfully done!$(NC)"

#ADD ONS_______________________________________________________
run: all
	@./$(NAME) $(ARGS) 2> test_errors.log

test: $(OBJS) $(TEST_OBJS)
	$(CXX) $(OBJS) $(TEST_OBJS) -o tester

$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) -I$(INCLUDES) -c $< -o $@

va: all
	@echo "\n\033[0;34mstart tester with valgrind...\033[0m\n"
	@valgrind \
	--leak-check=full \
	--track-origins=yes \
	--show-leak-kinds=all \
	./$(NAME) $(ARGS) 2> test_errors.log
	@echo

scan: all
	scan-build-12 \
	./$(NAME) $(ARGS)

fun: all
	funcheck -a \
	./$(NAME) $(ARGS)


-include $(DEPS)

#COLORS_____________________________________________________________
RED			=	\033[0;31m
GREEN		=	\033[0;32m
YELLOW		=	\033[0;33m
BLUE 		=	\033[0;34m
MAGENTA 	=	\033[0;35m
CYAN 		=	\033[0;36m
WHITE		=	\033[0;37m
NC			=	\033[0m
#BOLD COLORS________________________________________________________
BRED		=	\033[1;31m
BGREEN		=	\033[1;32m
BYELLOW		=	\033[1;33m
BBLUE 		=	\033[1;34m
BMAGENTA 	=	\033[1;35m
BCYAN 		=	\033[1;36m
BWHITE		=	\033[1;37m
