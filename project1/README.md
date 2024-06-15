# XTF Cryptocurrency Exchange Log Processor

The `xtf` script is designed to manage and process transaction logs for a cryptocurrency exchange platform, enabling effective sorting and filtering of user-specific data from mixed log files. 

## Key Functionalities

- **Transaction Listing:** Display filtered records for a specified user with options for listing by date, currency, or other attributes.
- **Currency Listing:** Output a sorted list of all currencies involved in the user's transactions.
- **Account Status:** Provide a detailed account balance grouped by currency, excluding fictional profit calculations.
- **Profit Calculation:** Optionally include a fictive profit in the account balances, adjustable through an environment variable for simulated high returns.

## Usage

```bash
xtf [-h|--help] [FILTR] [PŘÍKAZ] UŽIVATEL LOG [LOG2 [...]]
```

## Supported Commands
- `list`: Outputs user-specific transaction records.
- `list-currency`: Displays sorted list of currencies from user transactions.
- `status`: Shows actual account balance per currency.
- `profit`: Displays account balance including fictitious profits.

## Options
- `-a DATETIME`: Considers records after specified date/time.
- `-b DATETIME`: Considers records before specified date/time.
- `c CURRENCY`: Filters records for specified currency.
- `-h, --help`: Displays help information.

## Examples
```
$ ./xtf list Trader1 cryptoexchange.log
Trader1;2024-01-15 15:30:42;EUR;-2000.0000
Trader1;2024-01-16 18:06:32;USD;-3000.0000
Trader1;2024-01-20 11:43:02;ETH;1.9417
Trader1;2024-01-22 09:17:40;ETH;10.9537
```
```
$ ./xtf -c ETH Trader1 cryptoexchange.log
Trader1;2024-01-20 11:43:02;ETH;1.9417
Trader1;2024-01-22 09:17:40;ETH;10.9537
```
