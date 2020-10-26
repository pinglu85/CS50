import os
import re

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash
from dotenv import load_dotenv

from helpers import apology, login_required, lookup, usd

# Load .env
load_dotenv()

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True


# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    user_id = session["user_id"]

    # Query database for current user's cash
    cash = db.execute("SELECT cash FROM users WHERE id = :user_id",
                      user_id=user_id)[0]["cash"]

    # Grand total (stocks’ total value plus cash)
    grand_total = cash

    # Get current user owned stocks
    owned_stocks = get_owned_stocks(user_id)

    # If current user owns stocks
    if len(owned_stocks) != 0:
        # ...loop through all the stocks
        for stock in owned_stocks:
            # Look up the stock
            stock_info = lookup(stock["symbol"])
            # Stock current price
            stock_price = stock_info["price"]
            # Calculate total value of each holding (shares times price)
            stock_total_value = stock_price * stock["shares"]

            # Add stock's total value to grand total
            grand_total += stock_total_value

            # Store stock's company name to stock object
            stock["name"] = stock_info["name"]
            # Store stock's current price to stock object
            stock["price"] = usd(stock_price)
            # Store total value of each holding to stock object
            stock["total"] = usd(stock_total_value)

    # Render home page
    return render_template("index.html", stocks=owned_stocks, cash=usd(cash), grandTotal=usd(grand_total))


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    # User reached route via POST
    if request.method == 'POST':
        symbol = request.form.get("symbol").lower()
        # Ensure symbol is provided
        if not symbol:
            return apology("missing symbol", 400)

        shares = request.form.get("shares")
        # Ensure shares is provided
        if not shares:
            return apology("missing shares", 400)

        shares = int(shares)

        # Look up the stock
        stock_info = lookup(symbol)
        # Ensure stock is found
        if not stock_info:
            return apology("invalid symbol", 400)

        stock_price = stock_info["price"]
        stock_symbol = stock_info["symbol"]

        user_id = session["user_id"]

        # Query database for current user's cash
        cash = db.execute("SELECT cash FROM users WHERE id = :user_id",
                          user_id=user_id)[0]["cash"]
        remain_cash = cash - stock_price * shares

        # Ensure user can afford the number of shares at the current price.
        if remain_cash < 0:
            return apology("can't afford", 400)

        # Update current user's cash in database
        db.execute("UPDATE users SET cash = :remain_cash WHERE id = :user_id",
                   remain_cash=remain_cash, user_id=user_id)

        # Insert the transaction to table transactions in database
        db.execute("""
                   INSERT INTO
                      transactions (symbol, shares, price, user_id)
                   VALUES
                      (:symbol, :shares, :price, :user_id)
                   """,
                   symbol=stock_symbol, shares=shares, price=stock_price, user_id=user_id)

        # Flash message
        flash("Bought!")
        # Redirect user to home page
        return redirect("/")

    # User reached route via GET
    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    # Query database for current user's transaction history
    transactions = db.execute("""
                              SELECT 
                                  symbol, shares, price, transacted 
                              FROM 
                                  transactions 
                              WHERE user_id = :user_id
                              ORDER BY transacted 
                              """,
                              user_id=session["user_id"])

    # Format stock's price
    if len(transactions) != 0:
        for transaction in transactions:
            transaction["price"] = usd(transaction["price"])

    # Render history page
    return render_template("history.html", transactions=transactions)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

    # User reached route via POST
    if request.method == "POST":
        # Get symbol and lowercase it.
        symbol = request.form.get("symbol").lower()
        # Ensure symbol is provided
        if not symbol:
            return apology("missing symbol", 400)

        # Look up the stock
        stock_info = lookup(symbol)
        # Ensure stock is found
        if not stock_info:
            return apology("invalid symbol", 400)

        # Display result
        company = stock_info["name"]
        price = usd(stock_info["price"])
        symbol = stock_info["symbol"]
        return render_template("quoted.html", company=company, price=price, symbol=symbol)

    # User reached route via GET
    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    # User reached route via POST
    if request.method == 'POST':

        password = request.form.get("password")
        # Ensure password is provided
        if not password:
            return apology("missing password", 400)

        confirmation = request.form.get("confirmation")

        # Ensure passwords match
        if password != confirmation:
            return apology("passwords don't match", 400)

        username = request.form.get('username')
        # Insert username and hashed password into database
        user_id = db.execute("INSERT INTO users (username, hash) VALUES (:username, :hash)",
                             username=username, hash=generate_password_hash(password))

        # Remember which user has logged in
        session["user_id"] = user_id

        # Flashing message
        flash("Registered!")
        # Redirect user to home page
        return redirect("/")

    # User reached route via GET
    else:
        # Get query parameter
        username = request.args.get('username')

        # Check username availability if username is not empty.
        if username:
            # Query database for username
            rows = db.execute("SELECT * FROM users WHERE username = :username",
                              username=username)
            return jsonify({'availability': len(rows) == 0})

        # User reached route via GET without query parameter
        else:
            return render_template('register.html')


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    user_id = session["user_id"]
    # Get current user owned stocks
    owned_stocks = get_owned_stocks(user_id)

    # User reached route via POST
    if request.method == "POST":

        # Get stock's symbol current user want to sell
        symbol = request.form.get("symbol")
        # Ensure symbol is provided
        if not symbol:
            return apology("missing symbol", 400)

        # Ensure current user owns the stock
        owned_shares = list(
            filter(lambda stock: stock["symbol"] ==
                   symbol, owned_stocks))[0]["shares"]
        if not owned_shares or owned_shares <= 0:
            return apology("stock not owned", 400)

        # Get the number of shares current user want to sell
        sell_shares = request.form.get("shares")
        # Ensure the number of shares current user want to sell is provided
        if not sell_shares:
            return apology("missing shares", 400)

        sell_shares = int(sell_shares)

        # Ensure current user owns that many shares of the stock.
        if sell_shares > owned_shares:
            return apology("too many shares", 400)

        # Get stock info
        stock_info = lookup(symbol)
        # Ensure stock is found
        if not stock_info:
            return apology("invalid symbol", 400)

        stock_price = stock_info["price"]
        stock_symbol = stock_info["symbol"]

        # Insert the transaction into table transactions in database
        db.execute("""
                   INSERT INTO
                      transactions (symbol, shares, price, user_id)
                   VALUES
                      (:symbol, :shares, :price, :user_id)
                   """,
                   symbol=stock_symbol, shares=-sell_shares, price=stock_price, user_id=user_id)

        # Calculate total value
        total_value = stock_price * sell_shares
        # Query database for current user's cash
        cash = db.execute("SELECT cash FROM users WHERE id = :user_id",
                          user_id=user_id)[0]["cash"]
        # Update current user's cash
        cash += total_value
        # Update current user's cash in database
        db.execute("UPDATE users SET cash = :cash WHERE id = :user_id",
                   cash=cash, user_id=user_id)

        # Flash message
        flash("Sold!")
        # Redirect user to home page
        return redirect("/")

    # User reached route via GET
    else:
        return render_template("sell.html", stocks=owned_stocks)


@app.route("/add-cash", methods=["POST"])
@login_required
def add_cash():
    """ Add cash """

    cash_amount = request.form.get("cash-amount")
    # Ensure cash amount is provided
    if not cash_amount:
        return apology("missing cash amount", 400)

    cash_amount = float(cash_amount)

    user_id = session["user_id"]

    # Query database for current user's cash
    cash = db.execute("SELECT cash FROM users WHERE id = :user_id",
                      user_id=user_id)[0]["cash"]

    # Add cash amount to user's cash
    cash += cash_amount

    # Update current user's cash in database
    db.execute("UPDATE users SET cash = :cash WHERE id = :user_id",
               cash=cash, user_id=user_id)

    # Flash message
    flash("Cash added!")
    # Redirect user to home page
    return redirect("/")


@app.route("/change-password", methods=["GET", "POST"])
@login_required
def change_password():
    """Change password"""

    # User reached route via POST
    if request.method == "POST":

        current_password = request.form.get("current-password")
        # Ensure current password is provided
        if not current_password:
            return apology("missing current password")

        user_id = session["user_id"]

        # Ensure current password is correct
        hash = db.execute("SELECT hash FROM users WHERE id = :user_id",
                          user_id=user_id)[0]["hash"]
        if not check_password_hash(hash, current_password):
            return apology("your current password is incorrect", 400)

        new_password = request.form.get("new-password")
        # Ensure new password is provided
        if not new_password:
            return apology("missing new password", 400)

        confirmation = request.form.get("confirmation")
        # Ensure passwords match
        if new_password != confirmation:
            return apology("new passwords don't match", 400)

        # Ensure new password is different from current password
        if new_password == current_password:
            return apology("your new password must be different from your previous password.", 400)

        # Update hash in database
        db.execute("UPDATE users SET hash = :hash WHERE id = :user_id",
                   hash=generate_password_hash(new_password), user_id=user_id)

        # Forget any user_id
        session.clear()
        # Display change password success
        return render_template("change-password-success.html")

    # User reached route via GET
    else:
        return render_template("change-password.html")


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)


# Get user owned stocks
def get_owned_stocks(id):
    # Query database for stocks current user bought and sold,
    # group by stock's symbol and add up the number of shares
    # current user owns (bought - sold)
    stocks = db.execute("""
                        SELECT
                            symbol, SUM(shares) AS shares
                        FROM transactions
                        JOIN users ON users.id = transactions.user_id
                        WHERE users.id = :user_id
                        GROUP BY symbol
                        """,
                        user_id=id)
    # Filter out stocks current user owns
    return list(filter(lambda stock: stock["shares"] > 0, stocks))
