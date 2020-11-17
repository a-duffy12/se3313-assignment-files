/*
There seem to be a few errors in the given output
On 1992-10-19, the output states that the price of MSFT shares are $20.83, however this value does not appear in any of the obejcts in the input arrays
This leads to a situation where the user is said to incur a loss of $-1580 on 1992-10-23, however, since the value of the share should never have been updated to $20.83/share, the correct value is a loss of $-1080
My code reflects the values in the given input arrays, and therefore does not match the sample output in these two cases
*/

// input
actions = [{'date': '1992/07/14 11:12:30', 'action': 'BUY', 'price': '12.3', 'ticker': 'AAPL', 'shares': '500'}, {'date': '1992/09/13 11:15:20', 'action': 'SELL', 'price': '15.3', 'ticker': 'AAPL', 'shares': '100'}, {'date': '1992/10/14 15:14:20', 'action': 'BUY', 'price': '20', 'ticker': 'MSFT', 'shares': '300'}, {'date': '1992/10/17 16:14:30', 'action': 'SELL', 'price': '20.2', 'ticker': 'MSFT', 'shares': '200'}, {'date': '1992/10/19 15:14:20', 'action': 'BUY', 'price': '21', 'ticker': 'MSFT', 'shares': '500'}, {'date': '1992/10/23 16:14:30', 'action': 'SELL', 'price': '18.2', 'ticker': 'MSFT', 'shares': '600'}, {'date': '1992/10/25 10:15:20', 'action': 'SELL', 'price': '20.3', 'ticker': 'AAPL', 'shares': '300'}, {'date': '1992/10/25 16:12:10', 'action': 'BUY', 'price': '18.3', 'ticker': 'MSFT', 'shares': '500'}];
stock_actions = [{'date': '1992/08/14', 'dividend': '0.10', 'split': '', 'stock': 'AAPL'}, {'date': '1992/09/01', 'dividend': '', 'split': '3', 'stock': 'AAPL'}, {'date': '1992/10/15', 'dividend': '0.20', 'split': '', 'stock': 'MSFT'},{'date': '1992/10/16', 'dividend': '0.20', 'split': '', 'stock': 'ABC'}];

// order objects in actions and stock_actions
events = [];

// run function
outputTranscript(actions, stock_actions, events);

// function to output correct stuff
function outputTranscript(ac, sa, ev)
{
    let income = 0; // keep track of income
    let shares = []; // keep track of tickers and shared

    for (a in ac)
    {
        ev.push(ac[a]);
    }

    for (s in stock_actions)
    {
        ev.push(sa[s]);
    }

    events.sort(function(n,m) { // sort events by date
        return new Date(n.date) - new Date(m.date);
    });

    for (e in ev)
    {
        let owns = false; // bool to keep track of whether the user owns the affected stock or not
        let dao = new Date(ev[e].date); // turn given date into ISO format
        let dbo = dao.toISOString().substring(0, 10); // get only the day from the string
        
        console.log(`On ${dbo}, you have:`);

        if (ev[e].action == "BUY") // user is buying
        {
            let temp = {}; // create a temporary object
            temp.ticker = ev[e].ticker; // add the ticker to this object
            temp.count = ev[e].shares; // add share count to this object
            temp.price = ev[e].price; // add share price to this object

            for (let i=0; i<shares.length; i++)
            {
                if (shares[i].ticker == temp.ticker) // user already owns some of that share
                {
                    shares[i].count = parseInt(shares[i].count) + parseInt(temp.count); // update their number of shares
                    owns = true; // inform program that user already owns this share
                    break;
                }
            }
            
            if (!owns) // user doens't own any of that share
            {
                shares.push(temp); // add this share to list of user's shares
            }

            repeatOut(shares, income);
            console.log(`    - You bought ${ev[e].shares} shares of ${ev[e].ticker} at a price of $${parseFloat(ev[e].price).toFixed(2)} per share`);
        }
        else if (ev[e].action == "SELL") // user is selling
        {
            let ind = 0;

            for (let i=0; i<shares.length; i++)
            {
                if (shares[i].ticker == ev[e].ticker) // user already owns some of that share
                {
                    shares[i].count = parseInt(shares[i].count) - parseInt(ev[e].shares); // update their number of shares
                    owns = true; // inform program that user already owns this share
                    ind = i;
                    break;
                }
            }
            
            if (!owns) // user doens't own any of that share
            {
                shares.push(temp); // add this share to list of user's shares
            }

            netMoney = parseInt(ev[e].shares)*parseFloat(ev[e].price) - parseInt(ev[e].shares)*parseFloat(shares[ind].price); // shares*sellingPrice - shares*buyingPrice = net profit

            repeatOut(shares, income);

            if (netMoney > 0) // made a profit
            {
                console.log(`    - You sold ${ev[e].shares} shares of ${ev[e].ticker} at a price of $${parseFloat(ev[e].price).toFixed(2)} per share for a profit of $${netMoney.toFixed(2)}`);
            }
            else // made a loss
            {
                console.log(`    - You sold ${ev[e].shares} shares of ${ev[e].ticker} at a price of $${parseFloat(ev[e].price).toFixed(2)} per share for a loss of $${netMoney.toFixed(2)}`);
            }
        }
        else // it is a stock action
        {
            if (ev[e].dividend != "") // if dividends are paid
            {
                for (let i=0; i<shares.length; i++)
                {
                    if (shares[i].ticker == ev[e].stock) // user already owns some of that share
                    {
                        owns = true;
                        income += parseInt(shares[i].count) * parseFloat(ev[e].dividend); // add dividends to income value
                        repeatOut(shares, income);
                        console.log(`    - ${ev[e].stock} paid out $${parseFloat(ev[e].dividend).toFixed(2)} dividend per share, and you have ${shares[i].count} shares`);
                        break;
                    }
                }

                if (!owns)
                {
                    repeatOut(shares, income);
                    console.log(`    - ${ev[e].stock} paid out $${parseFloat(ev[e].dividend).toFixed(2)} dividend per share`);
                }
            }
            else if (ev[e].split != "") // if a stock is split
            {
                for (let i=0; i<shares.length; i++)
                {
                    if (shares[i].ticker == ev[e].stock) // user already owns some of that share
                    {
                        owns = true;
                        shares[i].count = parseInt(shares[i].count) * parseInt(ev[e].split); // update user's share count
                        shares[i].price = parseFloat(shares[i].price) / parseInt(ev[e].split); // update user's share price
                        repeatOut(shares, income);
                        console.log(`    - ${ev[e].stock} split ${ev[e].split} to 1, and you have ${shares[i].count} shares`);
                        break;
                    }
                }

                if (!owns)
                {
                    repeatOut(shares, income);
                    console.log(`    - ${ev[e].stock} split ${ev[e].split} to 1`);
                }
            }
        } 
    }
}           

// function to handle repeated output
function repeatOut(a, v)
{
    for (s in a)
    {
        if (a[s].count > 0)
        {
            console.log(`    - ${a[s].count} shares of ${a[s].ticker} at $${parseFloat(a[s].price).toFixed(2)} per share`);
        }
    }
    console.log(`    - $${v.toFixed(2)} of dividend income`);
    console.log(`  Transactions:`);
}