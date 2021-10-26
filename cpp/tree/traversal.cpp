#include <iostream>
#include <queue>
#include <stack>
using namespace std;

struct node
{
  node* left;
  node* right;
  char c;
};

void f(node* n){ cout << n->c << ' '; }

void in_order_r(node* root)
{
  if (!root) return;
  in_order_r(root->left);
  f(root);
  in_order_r(root->right);
}

void pre_order_r(node* root)
{
  if (!root) return;
  f(root);
  pre_order_r(root->left);
  pre_order_r(root->right);
}

void post_order_r(node* root)
{
  if (!root) return;
  post_order_r(root->left);
  post_order_r(root->right);
  f(root);
}

void breadth_r_helper(queue<node*>& q)
{
  if (q.empty()) return;
  auto* n = q.front();
  q.pop();
  if (n)
  {
    f(n);
    q.push(n->left);
    q.push(n->right);
  }
  breadth_r_helper(q);
}

void breadth_r(node* root)
{
  queue<node*> q;
  q.push(root);
  breadth_r_helper(q);
}

void in_order_i(node* root)
{
  stack<node*> s;
  node* cur = root;

  while(!s.empty() || cur)
  {
		while(cur)
		{
			s.push(cur);
			cur = cur->left;
		}
    cur = s.top();
    s.pop();
    f(cur);
    cur = cur->right;
  }
}

void pre_order_i(node* root)
{
  stack<node*> s;
  node* cur = root;

  while(!s.empty() || cur)
  {
		while(cur)
		{
      f(cur);
			s.push(cur);
			cur = cur->left;
		}
    cur = s.top();
    s.pop();
    cur = cur->right;
  }
}

void pre_order_i2(node* root)
{
	stack<node*> s;
	s.push(root);
	while(!s.empty())
	{
		node* cur = s.top();
    s.pop();
		if(cur)
		{
			f(cur);
			s.push(cur->right);
			s.push(cur->left);
		}
	}
}

void post_order_i(node* root)
{
  stack<node*> s;
  node* cur = root;

  while(!s.empty() || cur)
  {
		while(cur)
		{
			s.push(cur->right);
			s.push(cur);
			cur = cur->left;
		}

    cur = s.top();
    s.pop();

    if(!s.empty() && (cur->right == s.top()))
    {
      s.pop();
      s.push(cur);
      cur = cur->right;
    }
    else{
      f(cur);
			cur = nullptr;
    }
  }
}

void post_order_i2(node* root)
{
  stack<node*> s;
  node* cur = root;

  while(!s.empty() || cur)
  {
		while(cur)
		{
			s.push(cur);
			s.push(cur);
			cur = cur->left;
		}

    cur = s.top();
    s.pop();

    if(!s.empty() && (cur == s.top()))
    {
      cur = cur->right;
    }
    else{
      f(cur);
			cur = nullptr;
    }
  }
}

void post_order_i3(node* root)
{
  stack<node*> s;
  s.push(root);
  s.push(root);
  while(!s.empty())
  {
    node* n = s.top();
    s.pop();
    if(!n) continue;
    if(!s.empty() && n == s.top())
    {
      s.push(n->right);
      s.push(n->right);
      s.push(n->left);
      s.push(n->left);
    }
    else
    {
      f(n);
    }
  }
}

void breadth_i(node* root)
{
  queue<node*> q;
  q.push(root);
  while(!q.empty())
  {
    auto* n = q.front();
    q.pop();
    if(n)
    {
      f(n);
      q.push(n->left);
      q.push(n->right);
    }
  }
}

int main()
{
  node c{nullptr, nullptr, 'C'};
  node e{nullptr, nullptr, 'E'};
  node h{nullptr, nullptr, 'H'};
  node a{nullptr, nullptr, 'A'};
  node d{&c, &e, 'D'};
  node i{&h, nullptr, 'I'};
  node b{&a, &d, 'B'};
  node g{nullptr, &i, 'G'};
  node f{&b, &g, 'F'}; // ROOT

  // pre-order: F, B, A, D, C, E, G, I, H
  // in-order: A, B, C, D, E, F, G, H, I
  // post-order: A, C, E, D, B, H, I, G, F
  // Breadth-first: F, B, G, A, D, I, C, E, H

  //in_order_r(&f); cout << endl;
  //pre_order_r(&f); cout << endl;
  //post_order_r(&f); cout << endl;
  //breadth_r(&f); cout << endl;

  //in_order_i(&f); cout << endl;
  //pre_order_i(&f); cout << endl;
	//pre_order_i2(&f); cout << endl;
  post_order_i(&f); cout << endl;
	post_order_i2(&f); cout << endl;
  post_order_i3(&f); cout << endl;
  //breadth_i(&f); cout << endl;
  return 0;
}
