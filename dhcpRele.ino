#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

int qtd_desligamento = 0;

//Porta que vai ativar o rele
int porta_rele = 7;

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // this check is only needed on the Leonardo:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //Configurar porta de ativacao do rele para saida
  pinMode(porta_rele, OUTPUT);
  digitalWrite(porta_rele, LOW);

  Serial.println("Iniciando teste DHCP");
  while (Ethernet.begin(mac) == 0) {
    Serial.println("Falha ao receber IP do DHCP");
    if (qtd_desligamento < 5) {
      desligarLigarRoteador();
    }else{
      delay(3600000);
      qtd_desligamento = 0;
    }
  }

  // print your local IP address:
  printIPAddress();
}

void loop() {
  //Limita quantidade de desligamentos evitando danificar equipamento
  if (qtd_desligamento < 5) {
    switch (Ethernet.maintain()) {
      case 1:
        //renewed fail
        Serial.println("Error: renewed fail");
        desligarLigarRoteador();

        break;

      case 2:
        //renewed success
        Serial.println("Renewed success");
        //print your local IP address:
        printIPAddress();

        verificarInternet();
        break;

      case 3:
        //rebind fail
        Serial.println("Error: rebind fail");
        desligarLigarRoteador();
        break;

      case 4:
        //rebind success
        Serial.println("Rebind success");

        //print your local IP address:
        printIPAddress();

        verificarInternet();
        break;

      default:
        //nothing happened
        verificarInternet();
        delay(60000);
        break;
    }
  }else{
    //Caso tenha falhado 5x, esperar 1hora para reiniciar os testes
    delay(3600000);
    qtd_desligamento = 0;
  }
}




void verificarInternet() {
  int conectar = client.connect("google.com", 443);
  Serial.println("Client connect: " + (String)conectar);
  
  if (conectar == 1) {
    Serial.println("acessou google");
    client.stop();
    
    //Zerar contador apos sucesso
    qtd_desligamento = 0;
  }else if(conectar == 0){
    Serial.println("Sem conexao ethernet");
    desligarLigarRoteador();
  }else{
    Serial.println("Nao conseguiu acessar google");
    desligarLigarRoteador();
  }
  
}

void desligarLigarRoteador() {
  Serial.println("Desligando Roteador");
  digitalWrite(porta_rele, HIGH);
  delay(10000);
  Serial.println("Ligando Roteador apos 10s");
  digitalWrite(porta_rele, LOW);
  qtd_desligamento++;
  Serial.println("Desligamento... N = " + (String)qtd_desligamento);
  delay(30000);    
}

void printIPAddress()
{
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }

  Serial.println();
}
