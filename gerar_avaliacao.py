import pandas as pd

def generate_xls():
    # Ciclano, Beltrano, Fulano -> Keu, Luiz, Rafael
    data = {
        "Critérios": [
            "Completude (% do sprint backlog realizado)", 
            "Qualidade (% de funcionalidades testadas automatizadas)",
            "Uso de padrões de projeto de software (Singleton, DAO)",
            "Separação em camadas (MVC e headers separados)",
            "Componentização dos serviços (DLL/Shared Library)",
            "Boa organização do código (indentação e nomenclatura)",
            "Documentação automatizada (Doxygen)"
        ],
        "Keu": [100, 100, 100, 100, 100, 100, 100],
        "Luiz": [100, 100, 100, 100, 100, 100, 100],
        "Rafael": [100, 100, 100, 100, 100, 100, 100],
    }
    
    df = pd.DataFrame(data)
    
    # Calcular a média e adicionar a coluna Media
    df["Media"] = df[["Keu", "Luiz", "Rafael"]].mean(axis=1)
    
    # Salvar em formato Excel
    file_name = "Avaliacao_Sprint1.xlsx"
    df.to_excel(file_name, index=False)
    print(f"Arquivo '{file_name}' gerado com sucesso!")

if __name__ == "__main__":
    generate_xls()
