#include <stdio.h>
#include <mysql/mysql.h>

int main(void)
{
     MYSQL conexao;
     int res;

     mysql_init(&conexao);
     if ( mysql_real_connect(&conexao, "localhost", "guest", "guest", "teste", 0, NULL, 0) )
     {
        printf("conectado com sucesso!\n");
      
        res = mysql_query(&conexao,"INSERT INTO aprendendo(nome, sexo) values('Ricardo  Rodrigues Lucca', 'M');");

        if (!res) printf("Registros inseridos %d\n", mysql_affected_rows(&conexao));
        else printf("Erro na inserção %d : %s\n", mysql_errno(&conexao), mysql_error(&conexao));

        mysql_close(&conexao);
    }
    else
    {
        printf("Falha de conexao\n");
        printf("Erro %d : %s\n", mysql_errno(&conexao), mysql_error(&conexao));
     }
} 
