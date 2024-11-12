#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <cstdlib>
#include <ctime>

using namespace std;
// Classe TicTacToe
class TicTacToe {
private:
    std::array<std::array<char, 3>, 3> board; // Tabuleiro do jogo
    std::mutex board_mutex; // Mutex para controle de acesso ao tabuleiro
    std::condition_variable turn_cv; // Variável de condição para alternância de turnos
    char current_player; // Jogador atual ('X' ou 'O')
    bool game_over; // Estado do jogo
    char winner; // Vencedor do jogo
    bool inicio;

public:
    TicTacToe() {
        // Inicializar o tabuleiro e as variáveis do jogo
        for(int i=0; i<3; i++){
            this->board[i] = {' ', ' ', ' '};
        }
        this->game_over = false;
        this->winner = 'Z';
        this->inicio = true;
    }

    void display_board() {
        // Exibir o tabuleiro no console
        for(int i=0; i<3; i++){
            for(int j=0; j<3; j++){
                cout << " "<< this->board[i][j];
                if(j<2) cout << " |";    
            }
            if(i<2) cout << endl << "-----------" << endl;
        }
        cout << endl << endl;
        if(winner == 'Z') cout << "Courrent player " << this->current_player << endl << endl;
    }

    bool make_move(char player, int row, int col) {
        // Implementar a lógica para realizar uma jogada no tabuleiro
        // Utilizar mutex para controle de acesso
        unique_lock<mutex> lock(this->board_mutex);    
        if(this->inicio){
            if(this->board[row][col]== ' '){
                this->board[row][col]= player;
                this->display_board();
                this->inicio = false;
                if(player == 'X') this->current_player = 'O';
                else this->current_player = 'X';
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                return true;
            }
        }else {
        // Utilizar variável de condição para alternância de turnos

        if(player!=current_player) turn_cv.wait(lock);
        if(this->game_over) return false;
        if(this->board[row][col]== ' '){
            this->board[row][col]= player;
            if(this->check_win(player)){
                this->winner = this->current_player;
                this->game_over =true;
            }else if(this->check_draw()){
                this->winner = 'D';
                this->game_over = true;
            }
            if(this->current_player == 'X') this->current_player = 'O';
            else this->current_player = 'X';
            this->display_board();
            turn_cv.notify_one();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            return true;
        }
        // tentar fazer movimento, se nao for possivel, checa empate
        // se fizer movimento, checar vitoria
        // printa tabuleiro
        }
        return false;
    }

    bool check_win(char player) {
        // Verificar se o jogador atual venceu o jogo
        char cp = player;
        for(int i=0; i<3; i++){
            //testa linhas
            if(((this->board[i][0] == cp)&&(this->board[i][1]== cp)&&(this->board[i][2]== cp))||
                //testa colunas
               ((this->board[0][i] == cp)&&(this->board[1][i]== cp)&&(this->board[2][i]== cp))){
                return true;
            }
        }
        //testa diagonais
        if(((this->board[0][0] == cp)&&(this->board[1][1]== cp)&&(this->board[2][2]== cp))||
           ((this->board[2][0] == cp)&&(this->board[1][1]== cp)&&(this->board[0][2]== cp))){
            return true;
        }
        return false;
    }

    bool check_draw() {
        // Verificar se houve um empate
        for(int i=0; i<3; i++){
            for(int j=0; j<3; j++){
                if(this->board[i][j]==' ') return false;     
            }
        }
        return true;
    }

    bool is_game_over() {
        // Retornar se o jogo terminou
        return this->game_over;
    }

    char get_winner() {
        // Retornar o vencedor do jogo ('X', 'O', ou 'D' para empate)
        return this->winner;
    }
        
};

// Classe Player
class Player {
private:
    TicTacToe& game; // Referência para o jogo
    char symbol; // Símbolo do jogador ('X' ou 'O')
    std::string strategy; // Estratégia do jogador

public:
    Player(TicTacToe& g, char s, std::string strat) 
        : game(g), symbol(s), strategy(strat) {}

    void play() {
        while(!game.is_game_over()){
            if(strategy == "sequential"){
                this->play_sequential();
            }else if(strategy == "random"){
                this->play_random();
            }
        }
    }
        // Executar jogadas de acordo com a estratégia escolhida

private:
    void play_sequential() {
        // Implementar a estratégia sequencial de jogadas
        for (int i=0; i<3; i++) {
            for (int j=0; j<3; j++) {
                if(this->game.make_move(this->symbol, i, j)) return ;
            }
        }
    }

    void play_random() {
        // Implementar a estratégia aleatória de jogadas
        int row, col;
        while(!this->game.is_game_over()){
            row = rand() % 3;
            col = rand() % 3;
            if (game.make_move(this->symbol, row, col)) return;
        }
    }
};

// Função principal
int main() {
    unsigned seed = time(0);
    srand(seed);
    // Inicializar o jogo e os jogadores
    TicTacToe jogo;
    //jogo.display_board();
    Player invocador1(jogo, 'X', "random");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    Player invocador2(jogo, 'O', "random");
    // Criar as threads para os jogadores
    thread i1(&Player::play, &invocador1);
    thread i2(&Player::play, &invocador2);
    // Aguardar o término das threads
    i1.join();
    i2.join();

    // Exibir o resultado final do jogo
    if(jogo.get_winner() == 'D'){
        cout << "Empate!" << endl << endl; 
    } else 
        cout << "O jogador " << jogo.get_winner() << " ganhou!" << endl << endl;
    return 0;
}
