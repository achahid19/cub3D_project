#include "main.h"

t_bool space_checker(char *line)
{
	while (*line)
	{
		if (*line == '\n')
			return (true);
		if (*line != ' ')
			return (false);
		line++;
	}
	return (true);
}

t_bool ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r');
}

static void	ft_clear(char **ptr, int current)
{
	int	i;

	i = 0;
	while (i < current)
	{
		free(ptr[i]);
		i++;
	}
	free(ptr);
}

static int	checkwords(char const *s, char c)
{
	size_t	i;
	int		words;

	i = 0;
	words = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			while (s[i] && s[i] != c)
			{
				i++;
			}
			words++;
			continue ;
		}
		i++;
	}
	return (words);
}

static int	fill_arrays(char const *s, char c, char **ptr, int end)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (s[i])
	{
		if (s[i] == c)
		{
			i++;
			continue ;
		}
		else
		{
			end = i;
			while (s[end] && s[end] != c)
				end++;
			ptr[j] = ft_substr(s, i, end - i);
			if (ptr[j] == NULL)
				return (ft_clear(ptr, j), 1);
			j++;
			i = end;
		}
	}
	return (ptr[j] = NULL, 0);
}