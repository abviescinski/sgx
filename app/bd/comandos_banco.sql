
--CRIA CONTA
INSERT INTO cliente(nome, cpf, telefone) values('dados_cliente[0]','dados_cliente[1]','dados_cliente[2]');

SELECT cliente.ID_Cliente FROM cliente WHERE nome = 'dados_cliente[0]';

INSERT INTO conta(senha, saldo, tipo, C_ID_Cliente) values('dados_cliente[3]','100','dados_cliente[4]','dados_banco[0]');

SELECT conta.ID_Conta FROM conta INNER JOIN cliente ON cliente.ID_Cliente = dados_banco[0] AND conta.C_ID_Cliente = cliente.ID_Cliente;


--ACESSA CONTA
SELECT cliente.nome FROM conta INNER JOIN cliente ON conta.ID_conta = dados_cliente[0] AND conta.senha = dados_cliente[1]  AND conta.C_ID_Cliente = cliente.ID_Cliente;


--DEPOSITA
SELECT cliente.nome FROM conta INNER JOIN cliente ON conta.ID_conta = dados_cliente[0] AND conta.C_ID_Cliente = cliente.ID_Cliente;

UPDATE `conta` SET `saldo` = saldo + dados_cliente[1] WHERE `ID_Conta` = dados_cliente[0];

--VERIFICA SALDO
SELECT conta.saldo FROM conta WHERE conta.ID_conta = dados_acesso[0];

--SACAR
SELECT conta.saldo FROM conta WHERE conta.ID_conta = dados_acesso[0];

UPDATE `conta` SET `saldo` = saldo - dados_cliente[0] WHERE `ID_Conta` = dados_acesso[0];

SELECT conta.saldo FROM conta WHERE conta.ID_conta = dados_acesso[0];

--TRANSFERIR
SELECT conta.saldo FROM conta WHERE conta.ID_conta = dados_acesso[0];

SELECT cliente.nome FROM conta INNER JOIN cliente ON conta.ID_conta = dados_cliente[0] AND conta.C_ID_Cliente = cliente.ID_Cliente;

UPDATE `conta` SET `saldo` = saldo + dados_cliente[1] WHERE `ID_Conta` = dados_cliente[0];

UPDATE `conta` SET `saldo` = saldo - dados_cliente[1] WHERE `ID_Conta` = dados_acesso[0];

SELECT conta.saldo FROM conta WHERE conta.ID_conta = dados_acesso[0];
